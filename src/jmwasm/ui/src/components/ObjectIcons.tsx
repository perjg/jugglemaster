/** Simple single-color outline icons for the object selector (stroke = currentColor). */

interface IconProps {
  className?: string;
}

const svgProps = (className?: string) => ({
  className,
  viewBox: '0 0 24 24',
  fill: 'none',
  stroke: 'currentColor',
  strokeWidth: 1.5,
  strokeLinecap: 'round' as const,
  strokeLinejoin: 'round' as const,
  'aria-hidden': true,
});

/** Beanbag ball seen from top: panel seams cross at the pole, slightly off-center. */
export const IconBallSegmented = ({ className }: IconProps) => (
  <svg {...svgProps(className)}>
    <circle cx="12" cy="12" r="9" />
    <path d="M7.5 4.2 Q10.8 6.6 14.5 9.5 Q16.6 14.8 16.5 19.8" />
    <path d="M4.2 16.5 Q9.2 14.5 14.5 9.5 Q17.3 8.1 19.8 7.5" />
  </svg>
);

/** Juggling ring: slender annulus, filled between the two circles. */
export const IconRing = ({ className }: IconProps) => (
  <svg {...svgProps(className)}>
    <path
      fill="currentColor"
      fillRule="evenodd"
      stroke="none"
      d="M12 3a9 9 0 1 0 0 18 9 9 0 0 0 0-18Zm0 2.25a6.75 6.75 0 1 1 0 13.5 6.75 6.75 0 0 1 0-13.5Z"
    />
  </svg>
);

/** Fish (the animal), side view: almond body, forked tail, eye dot. */
export const IconFish = ({ className }: IconProps) => (
  <svg {...svgProps(className)}>
    <path d="M3 12 C6 7.5 12.5 7.5 16.5 11.2 L21 8.5 C20 10.5 20 13.5 21 15.5 L16.5 12.8 C12.5 16.5 6 16.5 3 12 Z" />
    <circle cx="6.8" cy="11.2" r="0.75" />
  </svg>
);

/** Club, horizontal: knob at left, thin handle, slender elongated body. */
export const IconClub = ({ className }: IconProps) => (
  <svg {...svgProps(className)}>
    <circle cx="2.6" cy="12" r="1.3" />
    <path d="M4.4 11.5 L9.5 11 C11.5 9.8 14.5 9.3 16.5 9.3 C19.9 9.3 22.4 10.4 22.4 12 C22.4 13.6 19.9 14.7 16.5 14.7 C14.5 14.7 11.5 14.2 9.5 13 L4.4 12.5" />
  </svg>
);
