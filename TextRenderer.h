#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h";

#include "PlayerUtils.h";

#include "Debug.h";

#include <cstdio>
#include <GL/glew.h>
#include <Windows.h>
#include <string>
#include <iostream>

/*
* Credit to VisualStudioHater for TextRenderer draw and init method reference.
* https://www.youtube.com/@VisualStudioHater
*/

/*
* YOU MUST ONLY CALL THIS METHOD IN RENDERTHREAD!
*/

class TextRenderer {
public:
	static inline unsigned char ttfBuffer[1 << 20];
	static inline unsigned char bitmap[512 * 512];

	static inline stbtt_bakedchar cdata[96];

	static inline const char* font;
	static inline float size;

	static inline GLuint ftex;

	static inline const char* entityDataToRenderName = NULL;
	static inline float entityDataToRenderHealth = NULL;
	static inline bool startDraw = false;

	static inline void init(int _size) {
		size = _size;
		Debug::write("initalizing font renderer..");

		FILE* file;
		fopen_s(&file, "c:/windows/fonts/seguivar.ttf", "rb"); // this can be whatever as long as it is a valid path. otherwise will crash.
		fread(ttfBuffer, 1, 1<<20, file);
		fclose(file);

		// bake the font
		stbtt_BakeFontBitmap(ttfBuffer, 0, size, bitmap, 512, 512, 32, 96, (stbtt_bakedchar*)cdata);

		// gl functions
		glGenTextures(1, &ftex);
		glBindTexture(GL_TEXTURE_2D, ftex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Debug::write("initalized font renderer. :)");
	}

	static inline void draw(float x, float y, const char* text, float* color) {
		glBindTexture(GL_TEXTURE_2D, ftex);
		glEnable(GL_TEXTURE_2D);
		glColor4f(color[0], color[1], color[2], color[3]);

		for (const char* p = text; *p; p++) {
			stbtt_bakedchar* b = cdata + (*p - 32);
			float xpos = x + b->xoff;
			float ypos = y + b->yoff;

			glBegin(GL_QUADS);
			glTexCoord2f(b->x0 / 512.0f, b->y0 / 512.0f); glVertex2f(xpos, ypos);
			glTexCoord2f(b->x1 / 512.0f, b->y0 / 512.0f); glVertex2f(xpos + b->x1 - b->x0, ypos);
			glTexCoord2f(b->x1 / 512.0f, b->y1 / 512.0f); glVertex2f(xpos + b->x1 - b->x0, ypos + b->y1 - b->y0);
			glTexCoord2f(b->x0 / 512.0f, b->y1 / 512.0f); glVertex2f(xpos, ypos + b->y1 - b->y0);
			glEnd();

			x += b->xadvance;
		}

		glDisable(GL_TEXTURE_2D);
	}

	static inline void drawWithShadow(float x, float y, const char* text, float* color) {
		glBindTexture(GL_TEXTURE_2D, ftex);
		glEnable(GL_TEXTURE_2D);

		for (const char* p = text; *p; p++) {
			stbtt_bakedchar* b = cdata + (*p - 32);
			float xpos = x + b->xoff + 1;
			float ypos = y + b->yoff + 1;

			glColor4f(0.28F, 0.28F, 0.28F, 0.8F);

			glBegin(GL_QUADS);
			glTexCoord2f(b->x0 / 512.0f, b->y0 / 512.0f); glVertex2f(xpos, ypos);
			glTexCoord2f(b->x1 / 512.0f, b->y0 / 512.0f); glVertex2f(xpos + b->x1 - b->x0, ypos);
			glTexCoord2f(b->x1 / 512.0f, b->y1 / 512.0f); glVertex2f(xpos + b->x1 - b->x0, ypos + b->y1 - b->y0);
			glTexCoord2f(b->x0 / 512.0f, b->y1 / 512.0f); glVertex2f(xpos, ypos + b->y1 - b->y0);
			glEnd();

			xpos = x + b->xoff;
			ypos = y + b->yoff;

			glColor4f(color[0], color[1], color[2], color[3]);

			glBegin(GL_QUADS);
			glTexCoord2f(b->x0 / 512.0f, b->y0 / 512.0f); glVertex2f(xpos, ypos);
			glTexCoord2f(b->x1 / 512.0f, b->y0 / 512.0f); glVertex2f(xpos + b->x1 - b->x0, ypos);
			glTexCoord2f(b->x1 / 512.0f, b->y1 / 512.0f); glVertex2f(xpos + b->x1 - b->x0, ypos + b->y1 - b->y0);
			glTexCoord2f(b->x0 / 512.0f, b->y1 / 512.0f); glVertex2f(xpos, ypos + b->y1 - b->y0);
			glEnd();

			x += b->xadvance;
		}

		glDisable(GL_TEXTURE_2D);
	}

	static inline void drawBox(float x, float y, float w, float h, float* color) {
		glColor4f(color[0], color[1], color[2], color[3]);

		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x, h);
		glVertex2f(w, h);
		glVertex2f(w, y);
		glEnd();
	}

	static inline void drawQueuedEntityData() {
		if (startDraw && entityDataToRenderHealth != NULL && entityDataToRenderName != NULL) {
			drawWithShadow((PlayerUtils::height / 2.0F) / 2.0F, (PlayerUtils::height / 2.0F) / 2.0F, entityDataToRenderName, new float[4] {1, 1, 1, 1});
			drawWithShadow((PlayerUtils::height / 2.0F) / 2.0F, (PlayerUtils::height / 2.0F) / 2.0F + 20, std::to_string(entityDataToRenderHealth).data(), new float[4] {1, 1, 1, 1});
		}
	}

	static inline void stopQueuedEntityData() {
		startDraw = false;
	}

	static inline void startQueuedEntityData() {
		startDraw = true;
	}
};