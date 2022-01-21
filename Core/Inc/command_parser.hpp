#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <cstring>
/**
 * @file command_parser.hpp
 * Single header library for parsing serial command. \n
 * Parser is using virtual function call.
 * No heap usage guarantee.
 */

namespace scp{

/// Common type to store all command option in array
struct option_base {
	/// virtual function call to parse command
	virtual bool parse(const char *buffor)const = 0;

protected:
	virtual ~ option_base() = default;
};


///  Option structure Using templated structure to avoid std::function and function pointers.
template <typename Functor>
struct option : public option_base {
	const Functor _function; ///< Deducted type of function to be executed if command match
	const char *const _argument; ///< Argument stored as a pointer to string literal
	const std::size_t _arg_size; ///< String lenght of _argument
	/**
	 *
	 * Constructor for option type. For example: \n
	 * option f1("-c", set_current); \n
	 * Make sure that you compiler support template argument deduction.
	 * @pre argument life time is longer than object itself.
	 * @param[in] argument string literal! eg. "-m". Object stores only pointer to argument.
	 * @param[in] functor function to be executed if command is correct.
	 * Function must have const char * as argument. \n
	 * If command is matched parser will pass pointer to value after command. \n
	 * Lambda type functions are suported.
	 *
	 */
	constexpr option(const char *argument, Functor functor)
	: _function{functor}, _arg_size{(strlen(argument))}, _argument{argument} {
	}


	/** Method for parsing string
	 * @param[in] buff pointer to char buffer with command to parse
	 * @return true if command is correct
	 */
	constexpr bool parse(const char *buff)const override{
		bool non_equal = strncmp(buff, _argument, _arg_size);
		if (!non_equal && ' '==*(buff + _arg_size)){
			_function(buff + _arg_size  + 1);
			return true;
		} else {
			return false;
		}
	}
};

}

#endif
