// swift-tools-version: 6.0
import PackageDescription

let package = Package(
    name: "JMKit",
    platforms: [.iOS(.v18), .tvOS(.v18), .macOS(.v15)],
    products: [
        .library(name: "JMKit", targets: ["JMKit"])
    ],
    targets: [
        .target(name: "JMKit"),
        .testTarget(
            name: "JMKitTests",
            dependencies: ["JMKit"],
            resources: [.copy("Fixtures")]
        ),
    ]
)
