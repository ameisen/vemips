require 'colorize'

module HueColorize
	module InstanceMethods
		def colorized_string(mode, color, background_color, previous)
			color_code = "\033[#{mode};#{color};#{background_color}m"

			previous = previous.rsub("\033[0m", color_code)

			if self.class.respond_to?(:enable_readline_support) && self.class.enable_readline_support
				return "\001#{color_code}\002#{previous}\001\033[0m"
			else
				return "#{color_code}#{previous}\033[0m"
			end
		end

		def colorize(params)
			return self if self.class.disable_colorization

			match = []
			colors_from_params(match, params)
			defaults_colors(match)
			match << self

			return colorized_string(match[0], match[1], match[2], match[3])
		end
	end
end

class String
	include HueColorize::InstanceMethods
end

=begin
module Hue
	COLOR_CODES = {
		black:         30,
		red:           31,
		green:         32,
		yellow:        33,
		blue:          34,
		magenta:       35,
		cyan:          36,
		white:         37,
		default:       39,
		light_black:   90,
		light_red:     91,
		light_green:   92,
		light_yellow:  93,
		light_blue:    94,
		light_magenta: 95,
		light_cyan:    96,
		light_white:   97
	}

	MODE_CODES = {
		default:          0, # Turn off all attributes
		bold:             1,
		dim:              2,
		italic:           3,
		underline:        4,
		blink:            5,
		blink_slow:       5,
		blink_fast:       6,
		invert:           7,
		hide:             8,
		strike:           9,
		double_underline: 20,
		reveal:           28,
		overlined:        53
	}
end

class String
	def with_color(mode, foreground, background)
		return "\e[#{mode};#{foreground};#{background}m#{self}\e[0m"
	end
end
=end
