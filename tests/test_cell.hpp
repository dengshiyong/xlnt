#pragma once

#include <ctime>
#include <iostream>
#include <cxxtest/TestSuite.h>

#include <xlnt/xlnt.hpp>

class test_cell : public CxxTest::TestSuite
{
public:
    void test_coordinates()
    {
        xlnt::cell_reference coord("ZF46");
        TS_ASSERT_EQUALS("ZF", coord.get_column());
        TS_ASSERT_EQUALS(46, coord.get_row());
    }

    void test_invalid_coordinate()
    {
        TS_ASSERT_THROWS(xlnt::cell_reference("AAA"), 
            xlnt::cell_coordinates_exception);
    }

    void test_zero_row()
    {
        TS_ASSERT_THROWS(xlnt::cell_reference("AQ0"), 
            xlnt::cell_coordinates_exception);
    }

    void test_absolute()
    {
        TS_ASSERT_EQUALS("$ZF$51", xlnt::cell_reference::make_absolute("ZF51").to_string());
    }

    void test_absolute_multiple()
    {
        TS_ASSERT_EQUALS("$ZF$51:$ZF$53", xlnt::range_reference::make_absolute("ZF51:ZF$53").to_string());
    }

    void test_column_index()
    {
        static const std::unordered_map<int, std::string> expected = 
        {
            {1, "A"},
            {10, "J"},
            {26, "Z"},
            {27, "AA"},
            {52, "AZ"},
            {53, "BA"},
            {78, "BZ"},
            {270, "jJ"},
            {677, "ZA"},
            {702, "ZZ"},
            {703, "AAA"},
            {728, "AAZ"},
            {731, "ABC"},
            {1353, "AZA"},
            {7030, "jjj"},
            {18253, "ZZA"},
            {18278, "ZZZ"}
        };

        for(auto expected_pair : expected)
        {
            TS_ASSERT_EQUALS(expected_pair.first, 
	        xlnt::cell_reference::column_index_from_string(expected_pair.second));
        }
    }

    void test_bad_column_index()
    {
        for(auto bad_string : {"JJJJ", "", "$", "1"})
        {
            TS_ASSERT_THROWS(xlnt::cell_reference::column_index_from_string(bad_string), xlnt::column_string_index_exception);
        }
    }

    void test_column_letter_boundaries()
    {
        TS_ASSERT_THROWS(xlnt::cell_reference::column_string_from_index(0),
	    xlnt::column_string_index_exception);
        TS_ASSERT_THROWS(xlnt::cell_reference::column_string_from_index(18279),
	    xlnt::column_string_index_exception);
    }


    void test_column_letter()
    {
        TS_ASSERT_EQUALS("ZZZ", xlnt::cell_reference::column_string_from_index(18278));
        TS_ASSERT_EQUALS("JJJ", xlnt::cell_reference::column_string_from_index(7030));
        TS_ASSERT_EQUALS("AB", xlnt::cell_reference::column_string_from_index(28));
        TS_ASSERT_EQUALS("AA", xlnt::cell_reference::column_string_from_index(27));
        TS_ASSERT_EQUALS("Z", xlnt::cell_reference::column_string_from_index(26));
    }


    void test_initial_value()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1", "17.5");

        TS_ASSERT(cell.get_value().is(xlnt::value::type::string));
    }

    void test_1st()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        TS_ASSERT(cell.get_value().is(xlnt::value::type::null));
    }

    void test_null()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1", "17.5");
        cell.set_value(xlnt::value::null());

        TS_ASSERT(cell.get_value().is(xlnt::value::type::null));
    }

    void test_numeric()
    {
        xlnt::workbook wb_guess_types;
        wb.set_guess_types(true);
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        cell.set_value(42);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("4.2");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("-42.000");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("0");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value(0);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value(0.0001);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("0.9999");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("99E-02");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value(1e1);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("4");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("-1E3");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value(4);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));
    }

    void test_string()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        cell.set_value("hello");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::string));
    }

    void test_single_dot()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_value(".");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::string));
    }

    void test_formula()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_formula("42");
        TS_ASSERT(cell.has_formula());
        cell.clear_formula();
        TS_ASSERT(!cell.has_formula());
        cell.set_formula("if(A1<4;-1;1)");
        TS_ASSERT(cell.has_formula());
    }

    void test_boolean()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_value(true);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::boolean));
        cell.set_value(false);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::boolean));
    }

    void test_leading_zero()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_value("0800");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::string));
    }

    void test_error_codes()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        for(auto error : xlnt::cell::ErrorCodes)
        {
            cell.set_error(error.first);
            TS_ASSERT(cell.get_value().is(xlnt::value::type::error));
        }
    }

    void test_insert_float()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_value(3.14);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));
    }

    void test_insert_percentage()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_value("3.14%");
        TS_ASSERT_DELTA(0.0314, cell.get_value().as<double>(), 1e-7);
    }

    void test_insert_datetime()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_value(xlnt::date::today());
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));
    }

    void test_insert_date()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_value(xlnt::datetime::now());
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));
    }

    void test_internal_date()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        xlnt::datetime dt(2010, 7, 13, 6, 37, 41);
        cell.set_value(dt);
        TS_ASSERT_EQUALS(40372.27616898148, cell.get_value().as<double>());
    }

    void test_datetime_interpretation()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        xlnt::datetime dt(2010, 7, 13, 6, 37, 41);
        cell.set_value(dt);
        TS_ASSERT_EQUALS(cell.get_value(), dt);
        TS_ASSERT_DELTA(cell.get_value().as<double>(), 40372.27616898148, 1e-7);
    }

    void test_date_interpretation()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        xlnt::date dt(2010, 7, 13);
        cell.set_value(dt);
        TS_ASSERT_EQUALS(cell.get_value(), dt);
        TS_ASSERT_EQUALS(cell.get_value().as<int>(), 40372);
    }

    void test_number_format_style()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        cell.set_value("12.6%");
        TS_ASSERT_EQUALS(xlnt::number_format::format::percentage, cell.get_style().get_number_format().get_format_code());
    }

    void test_data_type_check()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        TS_ASSERT(cell.get_value().is(xlnt::value::type::null));

        cell.set_value(".0e000");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("-0.e-0");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));

        cell.set_value("1E");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::string));
    }

    void test_set_bad_type()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        TS_ASSERT_THROWS(cell.set_error("1"), xlnt::data_type_exception);
        TS_ASSERT_THROWS(cell.set_hyperlink("1"), xlnt::data_type_exception);
    }


    void test_time()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        cell.set_value("03:40:16");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));
        TS_ASSERT_EQUALS(cell.get_value(), xlnt::time(3, 40, 16));

        cell.set_value("03:40");
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));
        TS_ASSERT_EQUALS(cell.get_value(), xlnt::time(3, 40));
    }
    
    void test_timedelta()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        
        cell.set_value(xlnt::timedelta(1, 3, 0, 0, 0));
        TS_ASSERT_EQUALS(cell.get_value(), 1.125);
        TS_ASSERT(cell.get_value().is(xlnt::value::type::numeric));
    }

    void test_date_format_on_non_date()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        cell.set_value(xlnt::datetime::now());
        cell.set_value("testme");
        TS_ASSERT("testme" == cell.get_value());
    }

    void test_set_get_date()
    {
        xlnt::datetime today(2010, 1, 18, 14,15, 20 ,1600);

        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        cell.set_value(today);
        TS_ASSERT(today == cell.get_value());
    }

    void test_repr()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        TS_ASSERT_EQUALS(cell.to_string(), "<Cell " + ws.get_title() + ".A1>");
    }

    void test_is_date()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        cell.set_value(xlnt::datetime::now());
        TS_ASSERT(cell.is_date());

        cell.set_value("testme");
        TS_ASSERT_EQUALS("testme", cell.get_value());
        TS_ASSERT(!cell.is_date());
    }


    void test_is_not_date_color_format()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");

        cell.set_value(-13.5);
        cell.get_style().get_number_format().set_format_code("0.00_);[Red]\\(0.00\\)");

        TS_ASSERT(!cell.is_date());
    }
    
    void test_comment_count()
    {
        xlnt::worksheet ws = wb.create_sheet();
        xlnt::cell cell(ws, "A1");
        
        TS_ASSERT(ws.get_comment_count() == 0);
        cell.set_comment(xlnt::comment("text", "author"));
        TS_ASSERT(ws.get_comment_count() == 1);
        cell.set_comment(xlnt::comment("text", "author"));
        TS_ASSERT(ws.get_comment_count() == 1);
        cell.clear_comment();
        TS_ASSERT(ws.get_comment_count() == 0);
        cell.clear_comment();
        TS_ASSERT(ws.get_comment_count() == 0);
    }
    
    void test_cell_offset()
    {
        xlnt::worksheet ws = wb.create_sheet();
        TS_ASSERT(ws.get_cell("B15").offset(1, 2).get_reference() == "C17");
    }

private:
    xlnt::workbook wb;
};
