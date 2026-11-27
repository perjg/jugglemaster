#!/usr/bin/env python3
"""Renders the app icons (iOS, macOS, tvOS layered icon and Top Shelf) into the asset catalogs.

Requires Pillow. Run from anywhere: scripts/icons/make-icons.py
"""

import json
import os
import shutil
import subprocess
import tempfile

from PIL import Image, ImageDraw, ImageFilter

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))
JMLIB = os.path.normpath(os.path.join(ROOT, "..", "jmlib"))

JMLIB_SOURCES = [
    "jmlib.cpp", "jmlib_wrapper.cpp", "jm_capi.cpp", "render_jm_flat.cpp", "util.cpp", "validator.cpp",
    "prefs.cpp", "jugglesaver/engine.cpp", "jugglesaver/shader_manager.cpp", "jugglesaver/jmlib_jsaver.cpp",
    "jugglesaver/js_validator.cpp", "jugglesaver/js_passing.cpp", "jugglesaver/render_common.cpp", "jugglesaver/gltrackball.cpp",
    "jugglesaver/trackball.cpp",
]

# x y z rotation, primary rgb, alternate rgb (engine colours of balls 0-2), back to front
RED = "0.168 0.368 -0.3 60 0.9 0 0 0 0.7 0"
BLUE = "-0.232 0.038 0 60 0 0 0.9 0 0.9 0.9"
GREEN = "0.168 -0.232 0.3 60 0 0.7 0 0 0 0.9"
BALLS = [RED, BLUE, GREEN]

BACKGROUND_TOP = (28, 32, 48)
BACKGROUND_BOTTOM = (8, 8, 14)

# Vertical field of view; a wider one leaves more margin around the balls
ICON_FOV = 30
TV_ICON_FOV = 35
TOP_SHELF_FOV = 40

INFO = {"author": "xcode", "version": 1}


def build_renderer(work):
    binary = os.path.join(work, "iconrender")
    sources = [os.path.join(HERE, "iconrender.cpp")] + [os.path.join(JMLIB, s) for s in JMLIB_SOURCES]
    subprocess.run(
        ["clang++", "-std=c++17", "-O2", "-w", "-fsigned-char", "-DJUGGLESAVER_SUPPORT", "-DUSE_SHADER_RENDERING",
         "-DGL_SILENCE_DEPRECATION", f"-I{JMLIB}", f"-I{JMLIB}/jugglesaver", *sources,
         "-framework", "OpenGL", "-framework", "ApplicationServices", "-o", binary],
        check=True)
    return binary


class Renderer:
    def __init__(self, binary, work):
        self.binary = binary
        self.work = work
        self.count = 0

    def balls(self, width, height, fov, balls):
        self.count += 1
        path = os.path.join(self.work, f"render{self.count}.png")
        args = [self.binary, path, str(width), str(height), str(fov)] + " ".join(balls).split()
        env = dict(os.environ)
        # Keeps the supersampled framebuffer within the GPU's renderbuffer limit
        if max(width, height) * 4 > 16384:
            env["JM_SUPERSAMPLE"] = "2"
        subprocess.run(args, check=True, env=env)
        return Image.open(path).convert("RGBA")


def background(width, height):
    image = Image.new("RGBA", (width, height))
    draw = ImageDraw.Draw(image)
    for y in range(height):
        t = y / max(1, height - 1)
        colour = tuple(round(a * (1 - t) + b * t) for a, b in zip(BACKGROUND_TOP, BACKGROUND_BOTTOM))
        draw.line([(0, y), (width, y)], fill=colour + (255,))
    return image


def flat(renderer, width, height, fov):
    image = background(width, height)
    image.alpha_composite(renderer.balls(width, height, fov, BALLS))
    return image.convert("RGB")


def write_json(directory, contents):
    os.makedirs(directory, exist_ok=True)
    with open(os.path.join(directory, "Contents.json"), "w") as f:
        json.dump(contents, f, indent=2)
        f.write("\n")


def catalog(directory):
    if not os.path.exists(os.path.join(directory, "Contents.json")):
        write_json(directory, {"info": INFO})


def install(staged, destination):
    """Replaces only the generated set, after everything rendered."""
    shutil.rmtree(destination, ignore_errors=True)
    shutil.move(staged, destination)


def mac_icon(renderer):
    """macOS shape: an 824 pt rounded square centred on the 1024 canvas with a soft shadow."""
    canvas, body, radius = 1024, 824, 185
    inset = (canvas - body) // 2
    mask = Image.new("L", (body, body), 0)
    ImageDraw.Draw(mask).rounded_rectangle([0, 0, body - 1, body - 1], radius=radius, fill=255)
    tile = flat(renderer, body, body, ICON_FOV).convert("RGBA")
    tile.putalpha(mask)

    shadow = Image.new("RGBA", (canvas, canvas), (0, 0, 0, 0))
    shadow_mask = Image.new("L", (canvas, canvas), 0)
    ImageDraw.Draw(shadow_mask).rounded_rectangle(
        [inset, inset + 12, inset + body - 1, inset + body + 11], radius=radius, fill=110)
    shadow.putalpha(shadow_mask.filter(ImageFilter.GaussianBlur(14)))

    icon = Image.new("RGBA", (canvas, canvas), (0, 0, 0, 0))
    icon.alpha_composite(shadow)
    icon.alpha_composite(tile, (inset, inset))
    return icon


def app_icons(renderer, staging):
    icon_set = os.path.join(staging, "AppIcon.appiconset")
    os.makedirs(icon_set)

    flat(renderer, 1024, 1024, ICON_FOV).save(os.path.join(icon_set, "ios-1024.png"))
    images = [{"filename": "ios-1024.png", "idiom": "universal", "platform": "ios", "size": "1024x1024"}]

    mac = mac_icon(renderer)
    for points in (16, 32, 128, 256, 512):
        for scale in (1, 2):
            name = f"mac-{points}@{scale}x.png"
            mac.resize((points * scale, points * scale), Image.LANCZOS).save(os.path.join(icon_set, name))
            images.append({"filename": name, "idiom": "mac", "scale": f"{scale}x", "size": f"{points}x{points}"})
    write_json(icon_set, {"images": images, "info": INFO})
    return icon_set


def image_set(directory, images):
    """images: [(scale, PIL image)]"""
    os.makedirs(directory)
    entries = []
    for scale, image in images:
        name = f"image@{scale}x.png"
        image.save(os.path.join(directory, name))
        entries.append({"filename": name, "idiom": "tv", "scale": f"{scale}x"})
    write_json(directory, {"images": entries, "info": INFO})


def image_stack(renderer, directory, width, height, scales):
    """Back: the background; then one layer per ball so each moves on its own in the parallax effect."""
    os.makedirs(directory)
    layers = [("Back", None), ("Red", [RED]), ("Blue", [BLUE]), ("Green", [GREEN])]
    for name, balls in layers:
        layer = os.path.join(directory, f"{name}.imagestacklayer")
        write_json(layer, {"info": INFO})
        images = []
        for scale in scales:
            w, h = width * scale, height * scale
            image = background(w, h).convert("RGB") if balls is None else renderer.balls(w, h, TV_ICON_FOV, balls)
            images.append((scale, image))
        image_set(os.path.join(layer, "Content.imageset"), images)
    write_json(directory, {
        "info": INFO,
        "layers": [{"filename": f"{name}.imagestacklayer"} for name, _ in reversed(layers)],
    })


def tv_icons(renderer, staging):
    brand = os.path.join(staging, "App Icon & Top Shelf Image.brandassets")
    os.makedirs(brand)

    image_stack(renderer, os.path.join(brand, "App Icon.imagestack"), 400, 240, (1, 2))
    image_stack(renderer, os.path.join(brand, "App Icon - App Store.imagestack"), 1280, 768, (1,))
    for name, width in (("Top Shelf Image", 1920), ("Top Shelf Image Wide", 2320)):
        image_set(os.path.join(brand, f"{name}.imageset"),
                  [(scale, flat(renderer, width * scale, 720 * scale, TOP_SHELF_FOV)) for scale in (1, 2)])

    write_json(brand, {
        "assets": [
            {"filename": "App Icon - App Store.imagestack", "idiom": "tv", "role": "primary-app-icon",
             "size": "1280x768"},
            {"filename": "App Icon.imagestack", "idiom": "tv", "role": "primary-app-icon", "size": "400x240"},
            {"filename": "Top Shelf Image Wide.imageset", "idiom": "tv", "role": "top-shelf-image-wide",
             "size": "2320x720"},
            {"filename": "Top Shelf Image.imageset", "idiom": "tv", "role": "top-shelf-image", "size": "1920x720"},
        ],
        "info": INFO,
    })
    return brand


def main():
    with tempfile.TemporaryDirectory() as work:
        renderer = Renderer(build_renderer(work), work)
        app_set = app_icons(renderer, work)
        tv_set = tv_icons(renderer, work)
        for staged, catalog_dir in ((app_set, os.path.join(ROOT, "App", "Assets.xcassets")),
                                    (tv_set, os.path.join(ROOT, "TV", "Assets.xcassets"))):
            catalog(catalog_dir)
            install(staged, os.path.join(catalog_dir, os.path.basename(staged)))


if __name__ == "__main__":
    main()
