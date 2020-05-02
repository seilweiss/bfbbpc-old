#include "zVar.h"

#include "xFont.h"

#include "print.h"

namespace
{
void parse_tag_var(xtextbox::jot &, const xtextbox &, const xtextbox &,
                   const xtextbox::split_tag &)
{
    BFBBSTUB("parse_tag_var");
}
}

void var_init()
{
    static const xtextbox::tag_type var_tag = { "var", 3, parse_tag_var, 0, 0 };

    xtextbox::register_tags(&var_tag, 1);
}