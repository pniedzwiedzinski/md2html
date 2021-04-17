/*
 * Source: https://github.com/kukrimate/md2html
 *
 * Copyright (c) 2020, Máté Kukri
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "stdio/stdio.h"

int
peak(FILE *fp)
{
	int ch;

	ungetc((ch = fgetc(fp)), fp);
	return ch;
}

int
next(FILE *fp, int want)
{
	int ch;

	ch = fgetc(fp);
	if (ch != want)
		ungetc(ch, fp);
	return ch;
}

char *
nextstr(FILE *fp, char *want)
{
	char *cur;
	int ch;

	for (cur = want; *cur; ++cur)
		if ((ch = fgetc(fp)) != *cur)
			goto fail;
	return want;
fail:
	ungetc(ch, fp);
	while (--cur >= want)
		ungetc(*cur, fp);
	return NULL;
}

void
skip_white(FILE *fp)
{
	int ch;

	while ((ch = fgetc(fp)) != EOF)
		switch (ch) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\v':
		case '\f':
			break;
		default:
			ungetc(ch, fp);
			return;
		}
}

void
code(FILE *fp)
{
	int ch;

	printf("<code>");
	while ((ch = fgetc(fp)) != EOF)
		switch (ch) {
		case '`':
			goto end;
		default:
			putchar(ch);
		}
end:
	printf("</code>");
}

void
esccode(FILE *fp)
{
	int ch;

	printf("<code>");
	while ((ch = fgetc(fp)) != EOF)
		switch (ch) {
		case '`':
			if (next(fp, '`') == '`')
				goto end;
		default:
			putchar(ch);
		}
end:
	printf("</code>");
}

void
codeblock(FILE *fp)
{
	int ch;

	skip_white(fp);
	printf("<pre><code>");
	while ((ch = fgetc(fp)) != EOF)
		switch (ch) {
		case '`':
			if (nextstr(fp, "``"))
				goto end;
		default:
			putchar(ch);
		}
end:
	printf("</code></pre>\n");
}

void
heading(FILE *fp, int lvl)
{
	int ch;

	skip_white(fp);
	printf("<h%d>", lvl);
	while ((ch = fgetc(fp)) != EOF)
		switch (ch) {
		/* End of heading */
		case '\n':
			goto end;
		default:
			putchar(ch);
		}
end:
	printf("</h%d>\n", lvl);
}

void
blockquote(FILE *fp)
{
	int ch;

	printf("<blockquote>");
	while ((ch = fgetc(fp)) != EOF)
		switch (ch) {
		case '\n':
			/* End of blockquote */
			if (next(fp, '\n') == '\n')
				goto end;

			/* Next top level element */
			switch (peak(fp)) {
			case '#': /* Heading */
			case '*': /* List */
				goto end;
			case '`': /* Code block */
				if (nextstr(fp, "```")) {
					ungetc('`', fp);
					ungetc('`', fp);
					ungetc('`', fp);
					goto end;
				}
			}

			/* Ignore > on subsequent lines */
			if (next(fp, '>') == '>')
				break;
		default:
			putchar(ch);
		}
end:
	printf("</blockquote>\n");
}

void
list(FILE *fp)
{
	int ch;

	printf("<ul><li>");

	while ((ch = fgetc(fp)) != EOF)
		switch (ch) {
		case '\n':
			/* End of list */
			if (next(fp, '\n') == '\n')
				goto end;

			/* Next top level element */
			switch (peak(fp)) {
			case '#': /* Heading */
			case '>': /* Blockquote */
				goto end;
			case '`': /* Code block */
				if (nextstr(fp, "```")) {
					ungetc('`', fp);
					ungetc('`', fp);
					ungetc('`', fp);
					goto end;
				}
			}

			/* Line starting with * means next element */
			if (next(fp, '*') == '*') {
				printf("</li><li>");
				break;
			}
		default:
			putchar(ch);
		}

end:
	printf("</li></ul>\n");
}

void
paragraph(FILE *fp)
{
	int ch;

	printf("<p>");
	while ((ch = fgetc(fp)) != EOF)
		switch (ch) {
		case '`':
			if (next(fp, '`') == '`')	/* Escaped code */
				esccode(fp);
			else				/* Normal code */
				code(fp);
			break;
		case '\n':
			/* End of paragraph */
			if (next(fp, '\n') == '\n')
				goto end;

			/* Next top level element */
			switch (peak(fp)) {
			case '#': /* Heading */
			case '>': /* Blockquote */
			case '*': /* List */
				goto end;
			case '`': /* Code block */
				if (nextstr(fp, "```")) {
					ungetc('`', fp);
					ungetc('`', fp);
					ungetc('`', fp);
					goto end;
				}
			}
		default:
			putchar(ch);
		}

end:
	printf("</p>\n");
}

void
md2html(FILE *fp)
{
	int ch, cnt;

	for (;;)
		switch ((ch = fgetc(fp))) {
		case EOF:
			return;
		/* Consume newlines at the start of top level blocks */
		case '\n':
			break;
		case '#':
			cnt = 1;
			while (next(fp, '#') == '#')
				++cnt;
			heading(fp, cnt);
			break;
		case '>':
			blockquote(fp);
			break;
		case '*':
			list(fp);
			break;
		case '`':
			if (nextstr(fp, "``")) {
				codeblock(fp);
				break;
			}
		default:
			ungetc(ch, fp);
			paragraph(fp);
		}
}

int
main(int argc, char *argv[])
{
	FILE *fp;

	if (argc < 2) {
		fp = stdin;
	} else {
		if (!(fp = fopen(argv[1], "r"))) {
			perror(argv[1]);
			return 1;
		}
	}

	md2html(fp);
	fclose(fp);
	return 0;
}
