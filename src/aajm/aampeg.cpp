#ifdef HAVE_AVCODEC_H

#include "aampeg.h"

/* x is 0 to 7, y is 0 to (font->height-1) */
/* Returns either 1 or 0 */
unsigned char getfontpixel(__AA_CONST struct aa_font *font, unsigned char c,
	unsigned char x, unsigned char y) {
    return (*(font->data + font->height * c + y) & (1<<x));
}


int creatempeg(JMLib *j, const char *filename) {

	aa_context *aacontext;
	AVCodec *codec;
	AVCodecContext *mpegcontext= NULL;
	AVFrame *picture;
	int i, out_size, size, outbuf_size;
	uint8_t *outbuf, *picture_buf;

	struct ball firstpos[BMAX]; /* remebmer where the balls started */
	int done = 0;
	int current_frames = 0;

	__AA_CONST struct aa_font *font;
	__AA_CONST unsigned char *attrs;
	int x, y; /* x and y position in the final image buffer */
	int xchar, ychar; /* x and y position in the text */
	int xbit, ybit; /* x and y position in the individual letter */
	unsigned char pixel; /* getfontpixel's value */
	int buf_pos;

	FILE *outputfile;

	outputfile = fopen(filename, "wb");
	if(outputfile == NULL) return 1;

	avcodec_init();
	avcodec_register_all();
	codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
	if (!codec) {
		return 1;
	}

	aacontext = aa_init(&mem_d,&aa_defparams,NULL);
	font = aa_currentfont(aacontext);;

	mpegcontext= avcodec_alloc_context();
	picture= avcodec_alloc_frame();
	mpegcontext->bit_rate = 400000;
	mpegcontext->width = aa_scrwidth(aacontext) * 9;
	mpegcontext->height = aa_scrheight(aacontext) * (font->height + 1);
	mpegcontext->frame_rate = 25;
	mpegcontext->frame_rate_base= 1;
	mpegcontext->gop_size = 30;
	mpegcontext->max_b_frames=1;

	if (avcodec_open(mpegcontext, codec) < 0) {
		return(1);
	}

	outbuf_size = 1000000;
	outbuf = (uint8_t *)malloc(outbuf_size);
	size = mpegcontext->width * mpegcontext->height;
	picture_buf = (uint8_t *)malloc((size * 3) / 2);

	picture->data[0] = picture_buf;
	picture->data[1] = picture->data[0] + size;
	picture->data[2] = picture->data[1] + size / 4;
	picture->linesize[0] = mpegcontext->width;
	picture->linesize[1] = mpegcontext->width / 2;
	picture->linesize[2] = mpegcontext->width / 2;


	for(i=j->balln-1;i>=0;i--) {
		firstpos[i] = j->b[i];
	}

	/* Loop here */
	while(!done) {
	
	j->doJuggle();
	done = 1;
	for(i=j->balln-1;i>=0;i--) {
		if(firstpos[i].gx != j->b[i].gx ||
			firstpos[i].gy != j->b[i].gy) done=0;
	}
	current_frames++;

	if(current_frames > 1000) done = 1;

	draw_juggler(0, aacontext, j);
	memset((void *)picture_buf, 16, (size*3)/2);
	memset((void *)picture->data[1], 128, size/4);
	memset((void *)picture->data[2], 128, size/4);

	for (xchar = 0; xchar < aa_scrwidth(aacontext); xchar++) {
		for (ychar = 0; ychar < aa_scrheight(aacontext); ychar++) {
			attrs = aa_attrs(aacontext);
			unsigned char attr = attrs[ychar * aa_scrwidth(aacontext) + xchar];
			unsigned char character = *(aa_text(aacontext) + ychar * aa_scrwidth(aacontext) + xchar);

			for (xbit = 0; xbit <= 7; xbit++) {
				for (ybit = 0; ybit <= font->height; ybit++) {
					x = xchar * 9 + (7 - xbit);
					y = ychar * (font->height+1) + ybit;
					buf_pos = mpegcontext->width * y + x;
					pixel = getfontpixel(font, character, xbit, ybit);
					switch(attr) {
						case AA_NORMAL:
							if(pixel) picture_buf[buf_pos] = 128;
							break;
						case AA_DIM:
							if(pixel) picture_buf[buf_pos] = 64;
							break;
						case AA_BOLD:
							if(pixel) picture_buf[buf_pos] = 196;
							break;
						case AA_BOLDFONT:
							if(pixel) picture_buf[buf_pos] = 196;
							break;
						case AA_REVERSE:
							if(pixel == 0) picture_buf[buf_pos] = 128;
							break;
						case AA_SPECIAL:
							if(pixel) picture_buf[buf_pos] = 235;
							break;
						default:
							break;
					}
				}
			}
		}
	}

	out_size = avcodec_encode_video(mpegcontext, outbuf, outbuf_size, picture);
	fwrite(outbuf, 1, out_size, outputfile);


	}
	/* End loop here */

	for(; out_size; i++) {
		out_size = avcodec_encode_video(mpegcontext, outbuf, outbuf_size, NULL);
		fwrite(outbuf, 1, out_size, outputfile);
	}

	outbuf[0] = 0x00;
	outbuf[1] = 0x00;
	outbuf[2] = 0x01;
	outbuf[3] = 0xb7;
	fwrite(outbuf, 1, 4, outputfile);
	fclose(outputfile);

	free(picture_buf);
	free(outbuf);

	free(mpegcontext);
	free(picture);

	return 0;
	/* this segfaults */
	aa_close(aacontext);

	/* this does, too */
	avcodec_close(mpegcontext);

	return(0);
}

#endif
