/*
 * TxtDisp.h
 *
 *  Created on: Mar 27, 2024
 *      Author: luca
 */

#ifndef TXTDISP_H_
#define TXTDISP_H_

#ifdef PC_ENVIRONMENT
#include <iostream>
#endif

#include <string>
#include <stdint.h>
#include <vector>
#include <cstring>

using namespace std;
/*
 * each parameter is identified by the pair (page,row)
 */
#define TD_MAX_PAGES				       	10
#define TD_MAX_PARAMS				       	20

#define TD_MAX_LABEL_LEN		       	8
#define TD_MAX_DESC_LEN			       	32
#define TD_MAX_UNIT_LEN			       	8
#define TD_STR_VALUE_SIZE		       	8
#define TD_FORMAT_SIZE			       	8
#define TD_DEF_PAGE_COLOR						TxtDisp::TD_COL_WHITE
#define TD_DEF_PARAM_COLOR					TxtDisp::TD_COL_WHITE
#define TD_GEN_DEF_FORMAT		       	"d"	// this format depends on the type
#define TD_INT_DEF_FORMAT		       	"0"
#define TD_FLOAT_DEF_FORMAT	       	".1"
#define TD_STR_DEF_FORMAT		       	"n"


#define TD_EDITABLE									true
#define TD_NOT_EDITABLE							false
#define TD_WRAP											true
#define TD_NO_WRAP									false
#define TD_HIDE											true
#define TD_SHOW											false


/*
 * FORMAT default depending from the type specified
 * "d"
 *
 * FORMAT for integer:
 * "0"	single value ex: 10, 22, 120, 1456, 0, -15
 * "0000" padded integer, in this case 0010, 0022, 0120, 1456, 0000, -0015
 *
 * FORMAT for float:
 * ".1"		ex: 0.1, 1.5, 13.7, 1240.8, 7.0, -0.9
 * ".3"		ex: 0.123, 1.541, 13.766, 1240.844, 7.000, -0.936
 *
 * FORMAT for string:
 * "n"		no formatting, will be printed "as is"
 * "U"		all uppercase
 * "l"		all lowecase
 */


/*
 * Error definitions
 */
enum 	td_errors_e
			{
			TD_SUCCESS,
			TD_UNK,
			TD_ERR_TOO_MANY_PAGES,
			TD_ERR_TOO_MANY_PARAMS,
			TD_ERR_LABEL_TOO_LONG,
			TD_ERR_DESC_TOO_LONG,
			TD_ERR_STR_VALUE_TOO_LONG,
			TD_ERR_UNIT_TOO_LONG,
			TD_ERR_FORMAT_TOO_LONG,
			TD_ERR_UNDEFINED_PAGE,
			TD_ERR_INVALID_DATA,
			};

/********************************************
 * Main class
 ********************************************/
class TxtDisp
	{
public:
	enum color_e
		{
		TD_COL_WHITE,
		TD_COL_BLACK,
		TD_COL_GRAY_LIGHT,
		TD_COL_GRAY,
		TD_COL_RED,
		TD_COL_GREEN,
		TD_COL_CYAN,
		TD_COL_BLUE,
		TD_COL_PURPLE,
		TD_COL_YELLOW,
		};

	/*
	 * value definition
	 */
	typedef uint16_t integer_t;
	typedef float real_t;
	typedef string string_t;

	struct value_t
		{
		enum type_e {T_INT, T_FLOAT, T_STR} type;
		union value_u
			{
			integer_t ivalue;
			real_t fvalue;
			char svalue[TD_STR_VALUE_SIZE];
			} value;

		// assignment overloading
		void operator =(const value_t &v)
			{
			type = v.type;
			switch(v.type)
				{
				case T_INT:
					value.ivalue = v.value.ivalue;
					break;
				case T_FLOAT:
					value.fvalue = v.value.fvalue;
					break;
				case T_STR:
					strcpy(value.svalue, v.value.svalue);
					break;
				}
			}
		} __attribute__ ((packed));

	/*
	 * parameter definition
	 */
	struct param_t
		{
		char desc[TD_MAX_DESC_LEN];
		char unit[TD_MAX_UNIT_LEN];
		char format[TD_FORMAT_SIZE];
		value_t value, def, max, min, step;
		color_e color;
		struct
			{
			uint8_t editable_f:		1;
			uint8_t wrap_f:				1;
			uint8_t hide_f:				1;
			uint8_t empty_f:			1;
			};
		} __attribute__ ((packed));

	/*
	 * page definition
	 */
	struct page_t
		{
		color_e color;
		char label[TD_MAX_LABEL_LEN];
		struct
			{
			uint8_t hide_f:			1;
			};
		uint8_t nParams;			// number of parameters in the array
		param_t params[TD_MAX_PARAMS];
		//vector<param_t> params;
		};

	//====================================
	// METHODS
	//====================================
	TxtDisp() {};
	virtual ~TxtDisp() {};

	/**
	 * add a page
	 * @param page_numb		page number
	 * @param label				page label
	 * @param color				color of the tab
	 * @param hide				true/false = hide/show
	 * @return error code (@see td_errors_e)
	 */
	td_errors_e addPage(uint8_t &page_numb, string label, color_e color=TD_DEF_PAGE_COLOR, bool hide=false)
	{
	td_errors_e ret = TD_UNK;
	page_t p;
	if(pages.size() < TD_MAX_PAGES)
		{
		if(label.size() < TD_MAX_LABEL_LEN)
			{
			p.color=color;
			p.nParams=0;
			strcpy(p.label,label.c_str());
			pages.push_back(p);
			page_numb = pages.size()-1;	// get the page number
			ret = TD_SUCCESS;
			}
		else
			{
			ret = TD_ERR_LABEL_TOO_LONG;
			}
		}
	else
		{
		ret = TD_ERR_TOO_MANY_PAGES;
		}
	return ret;
	}

	/**
	 * add a parameter (numeric version)
	 * @param page_numb		page number
	 * @param desc				parameter description
	 * @param type_val		value type (@see type_e)
	 * @param val_def			default value
	 * @param unit				measurement unit
	 * @param format			print format
	 * @param min					min value
	 * @param max					max value
	 * @param step				increment step
	 * @param color				color value (@see color_e)
	 * @param editable		is an editable parameter
	 * @param wrap				during editing values can wrap or not
	 * @param hide				true/false = hide/show
	 * @return
	 */
	template<typename T>
	td_errors_e addParam(
			uint8_t page_numb,
			string desc,
			value_t::type_e type_val,
			T val_def,
			string unit="",
			string format=TD_GEN_DEF_FORMAT,
			T min=0,
			T max=0,
			T step=0,
			color_e color=TD_DEF_PARAM_COLOR,
			bool editable=true,
			bool wrap=false,
			bool hide=false
			)
	{
	td_errors_e ret = TD_UNK;
	param_t p;
	try
		{
		if(page_numb > pages.size())	throw TD_ERR_UNDEFINED_PAGE;
		if(desc.size() > TD_MAX_DESC_LEN) throw TD_ERR_DESC_TOO_LONG;
		if(unit.size() > TD_MAX_UNIT_LEN) throw TD_ERR_UNIT_TOO_LONG;
		if(format.size() > TD_FORMAT_SIZE) throw TD_ERR_FORMAT_TOO_LONG;

		strcpy(p.desc,desc.c_str());

		p.value.type = type_val;
		p.def.type = type_val;
		p.min.type = type_val;
		p.max.type = type_val;

		switch(type_val)
			{
			case value_t::T_INT:
				if(format == "") format = TD_INT_DEF_FORMAT;
				p.value.value.ivalue = val_def;
				p.def.value.ivalue = val_def;
				p.min.value.ivalue = min;
				p.max.value.ivalue = max;
				p.step.value.ivalue = step;

				p.editable_f = editable;
				p.wrap_f = wrap;
				break;
			case value_t::T_FLOAT:
				if(format == "") format = TD_FLOAT_DEF_FORMAT;
				p.value.value.fvalue = val_def;
				p.def.value.fvalue = val_def;
				p.min.value.fvalue = min;
				p.max.value.fvalue = max;
				p.step.value.fvalue = step;

				p.editable_f = (editable) ? (1) : (0);
				p.wrap_f = (wrap) ? (1) : (0);
				p.hide_f = (hide) ? (1) : (0);
				break;
			default:
				throw TD_ERR_INVALID_DATA;
			}

		p.color = color;
		strcpy(p.format,format.c_str());

		//pages[page_numb].params.push_back(p);
		pages[page_numb].params[pages[page_numb].nParams] = p;
		pages[page_numb].nParams++;
		ret = TD_SUCCESS;
		}
	catch(td_errors_e &e)
		{
		ret = e;
		}

	return ret;
	}

	/**
	 * add a parameter (string specialized)
	 * @param page_numb		page number
	 * @param desc				parameter description
	 * @param val_def			default value
	 * @param unit				measurement unit
	 * @param format			print format
	 * @param color				color value (@see color_e)
	 * @param hide				true/false = hide/show
	 * @return error code (@see td_errors_e)
	 * - overload instead of specialization to avoid namespace -
	 */
	td_errors_e addParam(
			uint8_t page_numb,
			string desc,
			string_t val_def,
			string unit="",
			string format=TD_GEN_DEF_FORMAT,
			color_e color=TD_DEF_PARAM_COLOR,
			bool hide=false
			)
	{
	td_errors_e ret = TD_UNK;
	param_t p;
	try
		{
		if(page_numb > pages.size())	throw TD_ERR_UNDEFINED_PAGE;
		if(desc.size() > TD_MAX_DESC_LEN) throw TD_ERR_DESC_TOO_LONG;
		if(unit.size() > TD_MAX_UNIT_LEN) throw TD_ERR_UNIT_TOO_LONG;
		if(format.size() > TD_FORMAT_SIZE) throw TD_ERR_FORMAT_TOO_LONG;

		strcpy(p.desc,desc.c_str());

		p.value.type = value_t::T_STR;
		p.def.type = value_t::T_STR;
		p.min.type = value_t::T_STR;
		p.max.type = value_t::T_STR;

		if(format == "") format = TD_STR_DEF_FORMAT;

		if(val_def.size() > TD_STR_VALUE_SIZE) throw TD_ERR_STR_VALUE_TOO_LONG;
		strcpy(p.value.value.svalue, val_def.c_str());
		strcpy(p.def.value.svalue, val_def.c_str());
		p.min.value.svalue[0] = 0;
		p.max.value.svalue[0] = 0;
		p.step.value.svalue[0] = 0;

		p.editable_f = 0;	// forced to not editable
		p.wrap_f = 0;	// force to not wrap (not useful)
		p.hide_f = (hide) ? (1) : (0);

		p.color = color;
		strcpy(p.format, (format == TD_GEN_DEF_FORMAT) ? (TD_STR_DEF_FORMAT) : (format.c_str()));

		pages[page_numb].params[pages[page_numb].nParams] = p;
		pages[page_numb].nParams++;
		ret = TD_SUCCESS;
		}
	catch(td_errors_e &e)
		{
		ret = e;
		}

	return ret;
	}

	/**
	 * add an epmty row
	 * @return
	 */
	td_errors_e addEmptyRow()
	{
	td_errors_e ret = TD_UNK;
	// TODO
	return ret;
	}

	//=============================================================================
	// Communication
	//=============================================================================
	virtual td_errors_e sendPage(uint8_t page_numb)
	{
	td_errors_e ret = TD_UNK;
	// TODO
	return ret;
	}

	//=============================================================================
	// DEBUG
	//=============================================================================
#ifdef PC_ENVIRONMENT
	/*
	 * print pages (readable)
	 */
	void printPages()
	{
	cout << "PAGES:" << endl;
	for(const page_t& p : pages)
		{
		cout << "label=" << p.label << "; color=" << p.color << "; hide=" << ((p.hide_f) ? ("true") : ("false")) << endl;
		for(int par=0; par < p.nParams; par ++)
			{
			cout << "    desc=" << p.params[par].desc <<
					"; type=" << p.params[par].value.type <<
					"; format=" << p.params[par].format <<
					endl;
			}
		}
	}

	/*
	 * print the data stream (communication)
	 */
	void printDumpPage(int page_numb)
	{
#define MAX_BUFFER_SIZE				200
	char *hexstr = new char[MAX_BUFFER_SIZE];
	page_t p = pages[page_numb];

	cout << "max pages: " << TD_MAX_PAGES << "; max params per page: " << TD_MAX_PARAMS << endl;
	cout << "value size: " << sizeof(value_t) << endl;
	cout << "param size: " << sizeof(param_t) << endl;
	cout << "page size: " << sizeof(page_t) << endl;

	Hex2AsciiHex(hexstr, (const unsigned char *)&p, MAX_BUFFER_SIZE, sizeof(page_t), true, ' ');
	cout << "page stream [" << sizeof(page_t) << "]: " << hexstr << endl;
	delete [] hexstr;
	}

#endif

private:
	vector<page_t> pages;	// vector of pages data

	//=============================================================================
	// HELPERS
	//=============================================================================
#ifdef PC_ENVIRONMENT
	/**
	 * convert a byte vector to a string of hexadecimals
	 * @param dst buffer for the string result
	 * @param src buffer of the values to be converted
	 * @param len_dst number of data in dst
	 * @param len_src number of data in src
	 * @param sep true, add a separator
	 * @param sepChar separator char
	 * @return dst pointer
	 */
	char * Hex2AsciiHex(char *dst,const unsigned char *src, size_t len_dst, size_t len_src, bool sep, char sepChar)
	{
	*dst=(char) NULL;
	char tmp[3];
	char tmp1[2];

	tmp1[0]=sepChar;
	tmp1[1]=0;
	for(size_t i=0; i<len_src;i++)
		{
		sprintf(tmp,"%02X",((unsigned int)*(src+i) & 0xFF));
		if(sep) strcat(tmp,tmp1);
		strcat(dst,tmp);
		if(strlen(dst) >= (len_dst-6))
			{
			strcat(dst,"...");
			break;
			}
		}
	return(dst);
	}
#endif

	};

#endif /* TXTDISP_H_ */
