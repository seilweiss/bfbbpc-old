#include "xFont.h"

#include <rwcore.h>

#define SUBSTR(text) (text), sizeof((text)) - 1

namespace
{
void parse_tag_alpha(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void reset_tag_alpha(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void parse_tag_red(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti);
void reset_tag_red(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti);
void parse_tag_green(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void reset_tag_green(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void parse_tag_blue(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti);
void reset_tag_blue(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti);
void parse_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void reset_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void parse_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti);
void reset_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti);
void parse_tag_left_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					       const xtextbox::split_tag &ti);
void reset_tag_left_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					       const xtextbox::split_tag &ti);
void parse_tag_right_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						    const xtextbox::split_tag &ti);
void reset_tag_right_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						    const xtextbox::split_tag &ti);
void parse_tag_tab_stop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti);
void reset_tag_tab_stop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti);
void parse_tag_xspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti);
void reset_tag_xspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti);
void parse_tag_yspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti);
void reset_tag_yspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti);
void reset_tag_all(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti);
void parse_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void reset_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void parse_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti);
void reset_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti);
void parse_tag_wrap(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti);
void reset_tag_wrap(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti);
void parse_tag_xjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti);
void reset_tag_xjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti);
void parse_tag_yjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti);
void reset_tag_yjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti);
void parse_tag_open_curly(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						  const xtextbox::split_tag &ti);
void parse_tag_newline(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					   const xtextbox::split_tag &ti);
void parse_tag_tab(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti);
void parse_tag_word_break(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						  const xtextbox::split_tag &ti);
void parse_tag_page_break(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					      const xtextbox::split_tag &ti);
void parse_tag_model(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void reset_tag_model(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti);
void parse_tag_tex(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti);
void reset_tag_tex(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti);
void parse_tag_insert(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti);
void parse_tag_insert_hash(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						   const xtextbox::split_tag &ti);
void parse_tag_pop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti);
void parse_tag_timer(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				     const xtextbox::split_tag &ti);

// STUB
void parse_tag_alpha(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_alpha(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_red(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_red(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_green(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_green(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_blue(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_blue(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_width(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_height(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_left_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						   const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_left_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						   const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_right_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
							const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_right_indent(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
							const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_tab_stop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_tab_stop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_xspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_xspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_yspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_yspace(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_all(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_color(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_font(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_wrap(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_wrap(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_xjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_xjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_yjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_yjustify(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_open_curly(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						  const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_newline(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					   const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_tab(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_word_break(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						  const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_page_break(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						  const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_model(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_model(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_tex(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti)
{

}

// STUB
void reset_tag_tex(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_insert(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					  const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_insert_hash(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
						   const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_pop(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
				   const xtextbox::split_tag &ti)
{

}

// STUB
void parse_tag_timer(xtextbox::jot &a, const xtextbox &tb, const xtextbox &ctb,
					 const xtextbox::split_tag &ti)
{

}

xtextbox::tag_type format_tags_buffer[2][128] =
{
	SUBSTR(""), parse_tag_open_curly, NULL, NULL,
	SUBSTR("a"), parse_tag_alpha, reset_tag_alpha, NULL,
	SUBSTR("all"), NULL, reset_tag_all, NULL,
	SUBSTR("alpha"), parse_tag_alpha, reset_tag_alpha, NULL,
	SUBSTR("b"), parse_tag_blue, reset_tag_blue, NULL,
	SUBSTR("blue"), parse_tag_blue, reset_tag_blue, NULL,
	SUBSTR("c"), parse_tag_color, reset_tag_color, NULL,
	SUBSTR("color"), parse_tag_color, reset_tag_color, NULL,
	SUBSTR("f"), parse_tag_font, reset_tag_font, NULL,
	SUBSTR("font"), parse_tag_font, reset_tag_font, NULL,
	SUBSTR("g"), parse_tag_green, reset_tag_green, NULL,
	SUBSTR("green"), parse_tag_green, reset_tag_green, NULL,
	SUBSTR("h"), parse_tag_height, reset_tag_height, NULL,
	SUBSTR("height"), parse_tag_height, reset_tag_height, NULL,
	SUBSTR("i"), parse_tag_insert, NULL, NULL,
	SUBSTR("ih"), parse_tag_insert_hash, NULL, NULL,
	SUBSTR("insert"), parse_tag_insert, NULL, NULL,
	SUBSTR("left_indent"), parse_tag_left_indent, reset_tag_left_indent, NULL,
	SUBSTR("li"), parse_tag_left_indent, reset_tag_left_indent, NULL,
	SUBSTR("model"), parse_tag_model, reset_tag_model, NULL,
	SUBSTR("n"), parse_tag_newline, NULL, NULL,
	SUBSTR("newline"), parse_tag_newline, NULL, NULL,
	SUBSTR("page_break"), parse_tag_page_break, NULL, NULL,
	SUBSTR("pb"), parse_tag_page_break, NULL, NULL,
	SUBSTR("pop"), parse_tag_pop, NULL, NULL,
	SUBSTR("r"), parse_tag_red, reset_tag_red, NULL,
	SUBSTR("red"), parse_tag_red, reset_tag_red, NULL,
	SUBSTR("ri"), parse_tag_right_indent, reset_tag_right_indent, NULL,
	SUBSTR("right_indent"), parse_tag_right_indent, reset_tag_right_indent, NULL,
	SUBSTR("t"), parse_tag_tab, NULL, NULL,
	SUBSTR("tab"), parse_tag_tab, NULL, NULL,
	SUBSTR("tab_stop"), parse_tag_tab_stop, reset_tag_tab_stop, NULL,
	SUBSTR("tex"), parse_tag_tex, reset_tag_tex, NULL,
	SUBSTR("timer"), parse_tag_timer, NULL, NULL,
	SUBSTR("ts"), parse_tag_tab_stop, reset_tag_tab_stop, NULL,
	SUBSTR("w"), parse_tag_width, reset_tag_width, NULL,
	SUBSTR("wb"), parse_tag_word_break, NULL, NULL,
	SUBSTR("width"), parse_tag_width, reset_tag_width, NULL,
	SUBSTR("word_break"), parse_tag_word_break, NULL, NULL,
	SUBSTR("wrap"), parse_tag_wrap, reset_tag_wrap, NULL,
	SUBSTR("xj"), parse_tag_xjustify, reset_tag_xjustify, NULL,
	SUBSTR("xjustify"), parse_tag_xjustify, reset_tag_xjustify, NULL,
	SUBSTR("xs"), parse_tag_xspace, reset_tag_xspace, NULL,
	SUBSTR("xspace"), parse_tag_xspace, reset_tag_xspace, NULL,
	SUBSTR("yj"), parse_tag_xjustify, reset_tag_xjustify, NULL,
	SUBSTR("yjustify"), parse_tag_xjustify, reset_tag_xjustify, NULL,
	SUBSTR("ys"), parse_tag_xspace, reset_tag_xspace, NULL,
	SUBSTR("yspace"), parse_tag_xspace, reset_tag_xspace, NULL
};

xtextbox::tag_type *format_tags = format_tags_buffer[0];
unsigned int format_tags_size = 48;
}

void xtextbox::register_tags(const tag_type *t, unsigned int size)
{
	const tag_type *s1 = format_tags;
	const tag_type *s2 = t;
	const tag_type *end1 = format_tags + format_tags_size;
	const tag_type *end2 = t + size;

	format_tags = (format_tags == format_tags_buffer[0]) ?
		format_tags_buffer[1] : format_tags_buffer[0];

	tag_type *d = format_tags;

	while (s1 < end1 && s2 < end2)
	{
		int c = icompare(s1->name, s2->name);

		if (c < 0)
		{
			*d = *s1;
			s1++;
		}
		else if (c > 0)
		{
			*d = *s2;
			s2++;
		}
		else
		{
			*d = *s2;
			s1++;
			s2++;
		}

		d++;
	}

	while (s1 < end1)
	{
		*d = *s1;
		d++;
		s1++;
	}

	while (s2 < end2)
	{
		*d = *s2;
		d++;
		s2++;
	}

	format_tags_size = (unsigned int)(d - format_tags);
}