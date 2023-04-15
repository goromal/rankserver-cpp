#include "rankserver/PageBuilder.h"

std::optional<std::variant<HTML::Bold, HTML::Image>> renderFile(const std::string& fname)
{
    if (fname.size() <= 4)
    {
        return std::nullopt;
    }
    if (fname.substr(fname.size() - 4) == ".txt")
    {
        return HTML::Bold(fname);
    }
    else if (fname.substr(fname.size() - 4) == ".png")
    {
        std::string imgfname = std::string("/static/") + fname;
        return HTML::Image(imgfname, "", 0, 0, "(max-width: 500px) 100vw, (max-width: 900px) 50vw, 800px");
    }
    else
    {
        return std::nullopt;
    }
}

std::string buildRankserverPage(const bool&                     err,
                                const bool&                     done,
                                const std::string&              msg,
                                const std::vector<std::string>& rlist,
                                const std::string&              l,
                                const std::string&              r)
{
    HTML::Document document("Rank Server");
    document.addAttribute("lang", "en");

    document.head() << HTML::Meta("utf-8");
    document.head() << HTML::Style("img{width:100%;max-width:300px}") << HTML::Style(".column{float:left;width:50%}")
                    << HTML::Style(".row:after{content:'';display:table;clear:both}");

    document.body() << HTML::Header1("Rank Server");

    if (err)
    {
        document.body() << HTML::Paragraph("Server returned error:");
        document.body() << HTML::Paragraph(msg);
    }
    else
    {
        if (done)
        {
            document.body() << HTML::Paragraph("Sorting complete!");
            document.body() << (HTML::Form("/", "POST") << HTML::Input("submit", "z", "Refine Rankings"));
        }
        else
        {
            HTML::Div row("row");
            HTML::Div col1("column");
            HTML::Div col2("column");

            auto lval = renderFile(l);
            if (lval)
            {
                if (std::holds_alternative<HTML::Bold>(lval.value()))
                {
                    col1 << std::move(std::get<0>(lval.value()));
                }
                else
                {
                    col1 << std::move(std::get<1>(lval.value()));
                }
            }
            col1 << HTML::Element("br");
            col1 << (HTML::Form("/", "POST") << HTML::Input("submit", "l", "Choose"));

            auto rval = renderFile(r);
            if (rval)
            {
                if (std::holds_alternative<HTML::Bold>(rval.value()))
                {
                    col2 << std::move(std::get<0>(rval.value()));
                }
                else
                {
                    col2 << std::move(std::get<1>(rval.value()));
                }
            }
            col2 << HTML::Element("br");
            col2 << (HTML::Form("/", "POST") << HTML::Input("submit", "r", "Choose"));

            row << std::move(col1) << std::move(col2);
            document.body() << std::move(row);
        }
        document.body() << HTML::Element("hr");
        HTML::List ol(true);
        for (auto item : rlist)
        {
            auto ival = renderFile(item);
            if (ival)
            {
                if (std::holds_alternative<HTML::Bold>(ival.value()))
                {
                    ol << (HTML::ListItem() << std::move(std::get<0>(ival.value())));
                }
                else
                {
                    ol << (HTML::ListItem() << std::move(std::get<1>(ival.value())));
                }
            }
        }
        document.body() << std::move(ol);
    }

    return document.toString();
}
