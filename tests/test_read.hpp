#pragma once

#include <fstream>
#include <iostream>
#include <cxxtest/TestSuite.h>

#include <xlnt/xlnt.hpp>
#include "helpers/path_helper.hpp"

class test_read : public CxxTest::TestSuite
{
public:
    void test_read_standalone_worksheet()
    {
        auto path = PathHelper::GetDataDirectory("/reader/sheet2.xml");
        xlnt::workbook wb;
        xlnt::worksheet ws(wb);
        {
            std::ifstream handle(path);
            ws = xlnt::reader::read_worksheet(handle, wb, "Sheet 2", {"hello"});
        }
        TS_ASSERT_DIFFERS(ws, nullptr);
        if(!(ws == nullptr))
        {
            TS_ASSERT_EQUALS(ws.get_cell("G5").get_value(), "hello");
            TS_ASSERT_EQUALS(ws.get_cell("D30").get_value(), 30);
            TS_ASSERT_EQUALS(ws.get_cell("K9").get_value(), 0.09);
        }
    }
    
    xlnt::workbook standard_workbook()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/empty.xlsx");
        return xlnt::reader::load_workbook(path);
    }

    void test_read_standard_workbook()
    {
        TS_ASSERT_DIFFERS(standard_workbook(), nullptr);
    }

    void test_read_standard_workbook_from_fileobj()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/empty.xlsx");
        std::ifstream fo(path, std::ios::binary);
        auto wb = xlnt::reader::load_workbook(path);
        TS_ASSERT_DIFFERS(standard_workbook(), nullptr);
    }

    void test_read_worksheet()
    {
        auto wb = standard_workbook();
        auto sheet2 = wb.get_sheet_by_name("Sheet2 - Numbers");
        TS_ASSERT_DIFFERS(sheet2, nullptr);
        TS_ASSERT_EQUALS("This is cell G5", sheet2.get_cell("G5").get_value());
        TS_ASSERT_EQUALS(18, sheet2.get_cell("D18").get_value());
        TS_ASSERT_EQUALS(true, sheet2.get_cell("G9").get_value());
        TS_ASSERT_EQUALS(false, sheet2.get_cell("G10").get_value());
    }

    void test_read_nostring_workbook()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/empty-no-string.xlsx");
        auto wb = xlnt::reader::load_workbook(path);
        TS_ASSERT_DIFFERS(standard_workbook(), nullptr);
    }

    void test_read_empty_file()
    {
        auto path = PathHelper::GetDataDirectory("/reader/null_file.xlsx");
        TS_ASSERT_THROWS(xlnt::reader::load_workbook(path), xlnt::invalid_file_exception);
    }

    void test_read_empty_archive()
    {
        auto path = PathHelper::GetDataDirectory("/reader/null_archive.xlsx");
        TS_ASSERT_THROWS(xlnt::reader::load_workbook(path), xlnt::invalid_file_exception);
    }
    
    void test_read_workbook_with_no_properties()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/empty_with_no_properties.xlsx");
        xlnt::reader::load_workbook(path);
    }

    xlnt::workbook workbook_with_styles()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/empty-with-styles.xlsx");
        return xlnt::reader::load_workbook(path);
    }

    void test_read_workbook_with_styles_general()
    {
        auto wb = workbook_with_styles();
        auto ws = wb["Sheet1"];
        auto code = ws.get_cell("A1").get_style().get_number_format().get_format_code();
        auto expected = xlnt::number_format::format::general;
        TS_ASSERT_EQUALS(code, expected);
    }
    
    void test_read_workbook_with_styles_date()
    {
        auto wb = workbook_with_styles();
        auto ws = wb["Sheet1"];
        auto code = ws.get_cell("A2").get_style().get_number_format().get_format_code();
        auto expected = xlnt::number_format::format::date_xlsx14;
        TS_ASSERT_EQUALS(code, expected);
    }
    
    void test_read_workbook_with_styles_number()
    {
        auto wb = workbook_with_styles();
        auto ws = wb["Sheet1"];
        auto code = ws.get_cell("A3").get_style().get_number_format().get_format_code();
        auto expected = xlnt::number_format::format::number_00;
        TS_ASSERT_EQUALS(code, expected);
    }
    
    void test_read_workbook_with_styles_time()
    {
        auto wb = workbook_with_styles();
        auto ws = wb["Sheet1"];
        auto code = ws.get_cell("A4").get_style().get_number_format().get_format_code();
        auto expected = xlnt::number_format::format::date_time3;
        TS_ASSERT_EQUALS(code, expected);
    }
    
    void test_read_workbook_with_styles_percentage()
    {
        auto wb = workbook_with_styles();
        auto ws = wb["Sheet1"];
        auto code = ws.get_cell("A5").get_style().get_number_format().get_format_code();
        auto expected = xlnt::number_format::format::percentage_00;
        TS_ASSERT_EQUALS(code, expected);
    }
    
    xlnt::workbook date_mac_1904()
    {
        auto path = PathHelper::GetDataDirectory("/reader/date_1904.xlsx");
        return xlnt::reader::load_workbook(path);
    }
    
    xlnt::workbook date_std_1900()
    {
        auto path = PathHelper::GetDataDirectory("/reader/date_1900.xlsx");
        return xlnt::reader::load_workbook(path);
    }
    
    void test_read_win_base_date()
    {
        auto wb = date_std_1900();
        TS_ASSERT_EQUALS(wb.get_properties().excel_base_date, xlnt::calendar::windows_1900);
    }
    
    void test_read_mac_base_date()
    {
        auto wb = date_mac_1904();
        TS_ASSERT_EQUALS(wb.get_properties().excel_base_date, xlnt::calendar::mac_1904);
    }

    void test_read_date_style_win()
    {
        auto wb = date_std_1900();
        auto ws = wb["Sheet1"];
        TS_ASSERT_EQUALS(ws.get_cell("A1").get_style().get_number_format().get_format_code(), xlnt::number_format::format::date_xlsx14);
    }

    void test_read_date_style_mac()
    {
        auto wb = date_mac_1904();
        auto ws = wb["Sheet1"];
        TS_ASSERT_EQUALS(ws.get_cell("A1").get_style().get_number_format().get_format_code(), xlnt::number_format::format::date_xlsx14);
    }

    void test_read_compare_mac_win_dates()
    {
        auto wb_mac = date_mac_1904();
        auto ws_mac = wb_mac["Sheet1"];
        auto wb_win = date_std_1900();
        auto ws_win = wb_win["Sheet1"];
        xlnt::datetime dt(2011, 10, 31);
        TS_ASSERT_EQUALS(ws_mac.get_cell("A1").get_value(), dt);
        TS_ASSERT_EQUALS(ws_win.get_cell("A1").get_value(), dt);
        TS_ASSERT_EQUALS(ws_mac.get_cell("A1").get_value(), ws_win.get_cell("A1").get_value());
    }
    
    void test_repair_central_directory()
    {
        std::string data_a = "foobarbaz" + xlnt::reader::CentralDirectorySignature;
        std::string data_b = "bazbarfoo12345678901234567890";
        
        auto f = xlnt::reader::repair_central_directory(data_a + data_b);
        TS_ASSERT_EQUALS(f, data_a + data_b.substr(0, 18));
        
        f = xlnt::reader::repair_central_directory(data_b);
        TS_ASSERT_EQUALS(f, data_b);
    }
    
    void test_read_no_theme()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/libreoffice_nrt.xlsx");
        auto wb = xlnt::reader::load_workbook(path);
        TS_ASSERT_DIFFERS(wb, nullptr);
    }
    
    void test_read_cell_formulae()
    {
        xlnt::workbook wb;
        auto ws = wb.get_active_sheet();
        auto path = PathHelper::GetDataDirectory("/reader/worksheet_formula.xml");
        std::ifstream ws_stream(path);
        xlnt::reader::fast_parse(ws, ws_stream, {"", ""}, {}, 0);
        
        auto b1 = ws.get_cell("B1");
        TS_ASSERT(b1.has_formula());
        TS_ASSERT_EQUALS(b1.get_formula(), "CONCATENATE(A1,A2)");
        
        auto a6 = ws.get_cell("A6");
        TS_ASSERT(a6.has_formula());
        TS_ASSERT_EQUALS(a6.get_formula(), "SUM(A4:A5)");
    }
    
    void _test_read_complex_formulae()
    {
        /*
        auto path = PathHelper::GetDataDirectory("/reader/formulae.xlsx");
        auto wb = xlnt::reader::load_workbook(path);
        auto ws = wb.get_active_sheet();

        // Test normal forumlae
        TS_ASSERT(!ws.get_cell("A1").has_formula());
        TS_ASSERT(!ws.get_cell("A2").has_formula());
        TS_ASSERT(ws.get_cell("A3").has_formula());
        TS_ASSERT(ws.get_formula_attributes().find("A3") == ws.get_formula_attributes().end());
        TS_ASSERT(ws.get_cell("A3").get_formula() == "12345");
        TS_ASSERT(ws.get_cell("A4").has_formula());
        TS_ASSERT(ws.get_formula_attributes().find("A3") == ws.get_formula_attributes().end());
        ws.get_cell("A4").set_formula("A2+A3");
        TS_ASSERT(ws.get_cell("A5").has_formula());
        TS_ASSERT(ws.get_formula_attributes().find("A5") == ws.get_formula_attributes().end());
        ws.get_cell("A5").set_formula("SUM(A2:A4)");

        // Test unicode
        std::string expected = "=IF(ISBLANK(B16), \"D\xFCsseldorf\", B16)";
        TS_ASSERT(ws.get_cell("A16").get_formula() == expected);

        // Test shared forumlae
        TS_ASSERT(ws.get_cell("B7").get_data_type() == "f");
        TS_ASSERT(ws.formula_attributes["B7"]["t"] == "shared");
        TS_ASSERT(ws.formula_attributes["B7"]["si"] == "0");
        TS_ASSERT(ws.formula_attributes["B7"]["ref"] == "B7:E7");
        TS_ASSERT(ws.get_cell("B7").value == "=B4*2");
        TS_ASSERT(ws.get_cell("C7").get_data_type() == "f");
        TS_ASSERT(ws.formula_attributes["C7"]["t"] == "shared");
        TS_ASSERT(ws.formula_attributes["C7"]["si"] == "0");
        TS_ASSERT("ref" not in ws.formula_attributes["C7"]);
        TS_ASSERT(ws.get_cell("C7").value == "=");
        TS_ASSERT(ws.get_cell("D7").get_data_type() == "f");
        TS_ASSERT(ws.formula_attributes["D7"]["t"] == "shared");
        TS_ASSERT(ws.formula_attributes["D7"]["si"] == "0");
        TS_ASSERT("ref" not in ws.formula_attributes["D7"]);
        TS_ASSERT(ws.get_cell("D7").value == "=");
        TS_ASSERT(ws.get_cell("E7").get_data_type() == "f");
        TS_ASSERT(ws.formula_attributes["E7"]["t"] == "shared");
        TS_ASSERT(ws.formula_attributes["E7"]["si"] == "0");
        TS_ASSERT("ref" not in ws.formula_attributes["E7"]);
        TS_ASSERT(ws.get_cell("E7").value == "=");

        // Test array forumlae
        TS_ASSERT(ws.get_cell("C10").get_data_type() == "f");
        TS_ASSERT("ref" not in ws.formula_attributes["C10"]["ref"]);
        TS_ASSERT(ws.formula_attributes["C10"]["t"] == "array");
        TS_ASSERT("si" not in ws.formula_attributes["C10"]);
        TS_ASSERT(ws.formula_attributes["C10"]["ref"] == "C10:C14");
        TS_ASSERT(ws.get_cell("C10").value == "=SUM(A10:A14*B10:B14)");
        TS_ASSERT(ws.get_cell("C11").get_data_type() != "f");
        */
    }
    
    void test_data_only()
    {
        auto path = PathHelper::GetDataDirectory("/reader/formulae.xlsx");
        auto wb = xlnt::reader::load_workbook(path, false, true);
        auto ws = wb.get_active_sheet();

        TS_ASSERT(ws.get_formula_attributes().empty());
        TS_ASSERT(ws.get_parent().get_data_only());
        TS_ASSERT(ws.get_cell("A2").get_value().is(xlnt::value::type::numeric));
        TS_ASSERT(ws.get_cell("A2").get_value() == 12345);
        TS_ASSERT(!ws.get_cell("A2").has_formula());
        TS_ASSERT(ws.get_cell("A3").get_value().is(xlnt::value::type::numeric));
        TS_ASSERT(ws.get_cell("A3").get_value() == 12345);
        TS_ASSERT(!ws.get_cell("A3").has_formula());
        TS_ASSERT(ws.get_cell("A4").get_value().is(xlnt::value::type::numeric));
        TS_ASSERT(ws.get_cell("A4").get_value() == 24690);
        TS_ASSERT(!ws.get_cell("A4").has_formula());
        TS_ASSERT(ws.get_cell("A5").get_value().is(xlnt::value::type::numeric));
        TS_ASSERT(ws.get_cell("A5").get_value() == 49380);
        TS_ASSERT(!ws.get_cell("A5").has_formula());
    }
    
    void test_detect_worksheets()
    {
        {
            auto path = PathHelper::GetDataDirectory("/reader/bug137.xlsx");
            xlnt::zip_file archive(path);
            std::vector<std::pair<std::string, std::string>> expected =
            {
                {"xl/worksheets/sheet1.xml", "Sheet1"}
            };
            TS_ASSERT_EQUALS(xlnt::reader::detect_worksheets(archive), expected);
        }

        {
            auto path = PathHelper::GetDataDirectory("/reader/contains_chartsheets.xlsx");
            xlnt::zip_file archive(path);

            std::vector<std::pair<std::string, std::string>> expected =
            {
                {"xl/worksheets/sheet1.xml", "data"},
                {"xl/worksheets/sheet2.xml", "moredata"}
            };

            TS_ASSERT_EQUALS(xlnt::reader::detect_worksheets(archive), expected);
        }

        {
            auto path = PathHelper::GetDataDirectory("/reader/bug304.xlsx");
            xlnt::zip_file archive(path);

            std::vector<std::pair<std::string, std::string>> expected =
            {
                {"xl/worksheets/sheet3.xml", "Sheet1"},
                {"xl/worksheets/sheet2.xml", "Sheet2"},
                {"xl/worksheets/sheet.xml", "Sheet3"}
            };

            TS_ASSERT_EQUALS(xlnt::reader::detect_worksheets(archive), expected);
        }
    }
    
    void test_read_rels()
    {
        {
            std::vector<xlnt::relationship> expected =
            {
                {xlnt::relationship::type::theme, "rId3", "xl/theme/theme1.xml"},
                {xlnt::relationship::type::worksheet, "rId2", "xl/worksheets/sheet1.xml"},
                {xlnt::relationship::type::chartsheet, "rId1", "xl/chartsheets/sheet1.xml"},
                {xlnt::relationship::type::shared_strings, "rId5", "xl/sharedStrings.xml"},
                {xlnt::relationship::type::styles, "rId4", "xl/styles.xml"}
            };

            auto path = PathHelper::GetDataDirectory("/reader/bug137.xlsx");
            xlnt::zip_file archive(path);

            TS_ASSERT_EQUALS(xlnt::reader::read_relationships(archive, "xl/workbook.xml"), expected);
        }

        {
            std::vector<xlnt::relationship> expected =
            {
                {xlnt::relationship::type::custom_xml, "rId8", "../customXml/item3.xml"},
                {xlnt::relationship::type::worksheet, "rId3", "xl/worksheets/sheet.xml"},
                {xlnt::relationship::type::custom_xml, "rId7", "../customXml/item2.xml"},
                {xlnt::relationship::type::worksheet, "rId2", "xl/worksheets/sheet2.xml"},
                {xlnt::relationship::type::worksheet, "rId1", "xl/worksheets/sheet3.xml"},
                {xlnt::relationship::type::custom_xml, "rId6", "../customXml/item1.xml"},
                {xlnt::relationship::type::styles, "rId5", "xl/styles.xml"},
                {xlnt::relationship::type::theme, "rId4", "xl/theme/theme.xml"}
            };

            auto path = PathHelper::GetDataDirectory("/reader/bug304.xlsx");
            xlnt::zip_file archive(path);

            TS_ASSERT_EQUALS(xlnt::reader::read_relationships(archive, "xl/workbook.xml"), expected);
        }
    }
    
    void test_read_content_types()
    {
        std::vector<std::pair<std::string, std::string>> expected = 
        {
            {"/xl/workbook.xml", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml"},
            {"/xl/worksheets/sheet1.xml", "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml"},
            {"/xl/chartsheets/sheet1.xml", "application/vnd.openxmlformats-officedocument.spreadsheetml.chartsheet+xml"},
            {"/xl/worksheets/sheet2.xml", "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml"},
            {"/xl/theme/theme1.xml", "application/vnd.openxmlformats-officedocument.theme+xml"},
            {"/xl/styles.xml", "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml"},
            {"/xl/sharedStrings.xml", "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml"},
            {"/xl/drawings/drawing1.xml", "application/vnd.openxmlformats-officedocument.drawing+xml"},
            {"/xl/charts/chart1.xml", "application/vnd.openxmlformats-officedocument.drawingml.chart+xml"},
            {"/xl/drawings/drawing2.xml", "application/vnd.openxmlformats-officedocument.drawing+xml"},
            {"/xl/charts/chart2.xml", "application/vnd.openxmlformats-officedocument.drawingml.chart+xml"},
            {"/xl/calcChain.xml", "application/vnd.openxmlformats-officedocument.spreadsheetml.calcChain+xml"},
            {"/docProps/core.xml", "application/vnd.openxmlformats-package.core-properties+xml"},
            {"/docProps/app.xml", "application/vnd.openxmlformats-officedocument.extended-properties+xml"}
        };
        
        auto path = PathHelper::GetDataDirectory("/reader/contains_chartsheets.xlsx");
        xlnt::zip_file f(path);
        auto result = xlnt::reader::read_content_types(f);

	if(result.size() != expected.size())
	{
	    TS_ASSERT_EQUALS(result.size(), expected.size());
	    return;
	}

        for(std::size_t i = 0; i < expected.size(); i++)
        {
            TS_ASSERT_EQUALS(result[i], expected[i]);
        }
    }
    
    void test_read_sheets()
    {
        {
            auto path = PathHelper::GetDataDirectory("/reader/bug137.xlsx");
            xlnt::zip_file f(path);
            auto sheets = xlnt::reader::read_sheets(f);
            std::vector<std::pair<std::string, std::string>> expected =
            {{"rId1", "Chart1"}, {"rId2", "Sheet1"}};
            TS_ASSERT_EQUALS(sheets, expected);
        }
        
        {
            auto path = PathHelper::GetDataDirectory("/reader/bug304.xlsx");
            xlnt::zip_file f(path);
            auto sheets = xlnt::reader::read_sheets(f);
            std::vector<std::pair<std::string, std::string>> expected =
            {{"rId1", "Sheet1"}, {"rId2", "Sheet2"}, {"rId3", "Sheet3"}};
            TS_ASSERT_EQUALS(sheets, expected);
        }
    }
    
    void test_guess_types()
    {
        bool guess;
        xlnt::value::type dtype;
        std::vector<std::pair<bool, xlnt::value::type>> test_cases = {{true, xlnt::value::type::numeric}, {false, xlnt::value::type::string}};
        
        for(const auto &expected : test_cases)
        {
            std::tie(guess, dtype) = expected;
            auto path = PathHelper::GetDataDirectory("/genuine/guess_types.xlsx");
            auto wb = xlnt::reader::load_workbook(path, guess);
            auto ws = wb.get_active_sheet();
            TS_ASSERT(ws.get_cell("D2").get_value().is(dtype));
        }
    }
    
    void test_read_autofilter()
    {
        auto path = PathHelper::GetDataDirectory("/reader/bug275.xlsx");
        auto wb = xlnt::reader::load_workbook(path);
        auto ws = wb.get_active_sheet();
        TS_ASSERT_EQUALS(ws.get_auto_filter().to_string(), "A1:B6");
    }
    
    void test_bad_formats_xlsb()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/a.xlsb");
        TS_ASSERT_THROWS(xlnt::reader::load_workbook(path), xlnt::invalid_file_exception);
    }
    
    void test_bad_formats_xls()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/a.xls");
        TS_ASSERT_THROWS(xlnt::reader::load_workbook(path), xlnt::invalid_file_exception);
    }
    
    void test_bad_formats_no()
    {
        auto path = PathHelper::GetDataDirectory("/genuine/a.no-format");
        TS_ASSERT_THROWS(xlnt::reader::load_workbook(path), xlnt::invalid_file_exception);
    }
};
