require 'ffi/bit_field'

module AliasLayout
	extend FFI::BitField::Layout
	extend FFI::BitField::Property

	def bit_fields(*layout_args)
		super(*layout_args)

		@bit_field_hash_table.each { |k, v|
			self.define_method "#{k}=" do |arg|
				self[k] = arg
			end
			self.define_method "#{k}" do
				self[k]
			end
			puts "#{k} #{v}"
		}
	end
end

class BitField < FFI::BitStruct
	extend AliasLayout
end

class TypeClass < BitField
	layout \
		:value, :uint32

	bit_fields :value,
		:windows, 1,
		:mingw, 1,
		:cygwin, 1,
		:linux, 1,
		:macintosh, 1,
		:unix, 1
end
