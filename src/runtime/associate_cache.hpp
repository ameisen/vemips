#pragma once

#include "common.hpp"
#include <algorithm>
#include <optional>

/* With Optional */

template <typename KeyT, typename ValueT, size_t Associativity>
struct associate_cache final {
	struct key_value final {
		KeyT key = {};
		ValueT value = {};

		key_value& operator = (const key_value& other) = default;
	};

	mutable key_value values_[Associativity];
	mutable uint32 last_set_ = 0;

private:
	template <typename KeyTT>
	_forceinline _nothrow std::optional<ValueT> get_internal(KeyTT key) const {
		//const uint32 last_set = last_set_;

		for (size_t i = 0; i < Associativity; ++i) {
			key_value kv = values_[i];

			if (kv.key == key) {
				/*
				if (i != last_set) {
					values_[i] = values_[last_set];
					values_[last_set] = kv;
				}
				*/
				return kv.value;
			}
		}

		return {};
	}

	template <typename KeyTT, typename ValueTT>
	_forceinline _nothrow void set_internal(KeyTT key, ValueTT value) {
		uint32 last_set = last_set_ + 1;
		last_set %= Associativity;
		values_[last_set] = { .key = key, .value = value };
		last_set_ = last_set;
	}

public:
	_nothrow std::optional<ValueT> get(const KeyT & key) const {
		return get_internal(key);
	}
	_nothrow std::optional<ValueT> get(KeyT && key) const {
		return get_internal(std::forward<KeyT>(key));
	}

	_nothrow void set(const KeyT & key, const ValueT & value) {
		set_internal(key, value);
	}
	_nothrow void set(KeyT && key, const ValueT& value) {
		set_internal(std::forward<KeyT>(key), value);
	}
	_nothrow void set(const KeyT & key, ValueT && value) {
		set_internal(key, std::forward<ValueT>(value));
	}
	_nothrow void set(KeyT && key, ValueT && value) {
		set_internal(std::forward<KeyT>(key), std::forward<ValueT>(value));
	}
};

template <typename KeyT, typename ValueT>
struct associate_cache<KeyT, ValueT, 1> final {
	struct key_value final {
		KeyT key = {};
		ValueT value = {};
	};

	mutable key_value value;

	_nothrow std::optional<ValueT> get(const KeyT & key) const {
		return value.key == key ?
			std::make_optional(value.value) :
			std::nullopt;
	}

	_nothrow std::optional<ValueT> get(KeyT&& key) const {
		return value.key == key ?
			std::make_optional(value.value) :
			std::nullopt;
	}

	_nothrow void set(const KeyT & key, const ValueT & value) {
		this->value = { key, value };
	}
	_nothrow void set(KeyT && key, const ValueT & value) {
		this->value = { key, value };
	}
	_nothrow void set(const KeyT & key, ValueT && value) {
		this->value = { key, value };
	}
	_nothrow void set(KeyT && key, ValueT && value) {
		this->value = { key, value };
	}
};

template <typename KeyT, typename ValueT>
struct associate_cache<KeyT, ValueT, 0> final {
	_forceinline _nothrow std::optional<ValueT> get(const KeyT& key) const {
		return std::nullopt;
	}

	_forceinline _nothrow std::optional<ValueT> get(KeyT&& key) const {
		return std::nullopt;
	}

	_forceinline _nothrow void set(const KeyT& key, const ValueT& value) {
	}
	_forceinline _nothrow void set(KeyT&& key, const ValueT& value) {
	}
	_forceinline _nothrow void set(const KeyT& key, ValueT&& value) {
	}
	_forceinline _nothrow void set(KeyT&& key, ValueT&& value) {
	}
};

/* With Sentinel */

template <typename T>
using sentinel_converter_t = std::conditional_t<std::is_pointer_v<T>, uintptr, T>;

template <typename KeyT, typename ValueT, size_t Associativity, sentinel_converter_t<ValueT> Sentinel>
struct sentinel_associate_cache final {
	static _forceinline _nothrow bool is_sentinel(const ValueT& value) {
		return (decltype(Sentinel))(value) == Sentinel;
	}

	struct key_value final {
		KeyT key = {};
		ValueT value = {};

		key_value& operator = (const key_value& other) = default;
	};

	mutable key_value values_[Associativity];
	mutable uint32 last_set_ = 0;

private:
	template <typename KeyTT>
	_forceinline _nothrow ValueT get_internal(KeyTT key) const {
		//const uint32 last_set = last_set_;

		for (size_t i = 0; i < Associativity; ++i) {
			key_value kv = values_[i];

			if (kv.key == key) {
				/*
				if (i != last_set) {
					values_[i] = values_[last_set];
					values_[last_set] = kv;
				}
				*/
				return kv.value;
			}
		}

		return ValueT(Sentinel);
	}

	template <typename KeyTT, typename ValueTT>
	_forceinline _nothrow void set_internal(KeyTT key, ValueTT value) {
		uint32 last_set = last_set_ + 1;
		last_set %= Associativity;
		values_[last_set] = { .key = key, .value = value };
		last_set_ = last_set;
	}

public:
	_nothrow ValueT get(const KeyT& key) const {
		return get_internal(key);
	}
	_nothrow ValueT get(KeyT&& key) const {
		return get_internal(std::forward<KeyT>(key));
	}

	_nothrow void set(const KeyT& key, const ValueT& value) {
		set_internal(key, value);
	}
	_nothrow void set(KeyT&& key, const ValueT& value) {
		set_internal(std::forward<KeyT>(key), value);
	}
	_nothrow void set(const KeyT& key, ValueT&& value) {
		set_internal(key, std::forward<ValueT>(value));
	}
	_nothrow void set(KeyT&& key, ValueT&& value) {
		set_internal(std::forward<KeyT>(key), std::forward<ValueT>(value));
	}
};

template <typename KeyT, typename ValueT, sentinel_converter_t<ValueT> Sentinel>
struct sentinel_associate_cache<KeyT, ValueT, 1, Sentinel> final {
	static _forceinline _nothrow bool is_sentinel(const ValueT& value) {
		return (decltype(Sentinel))(value) == Sentinel;
	}

	struct key_value final {
		KeyT key = {};
		ValueT value = {};
	};

	mutable key_value value;

	_nothrow ValueT get(const KeyT& key) const {
		return value.key == key ?
			value.value :
			ValueT(Sentinel);
	}

	_nothrow ValueT get(KeyT&& key) const {
		return value.key == key ?
			value.value :
			ValueT(Sentinel);
	}

	_nothrow void set(const KeyT& key, const ValueT& value) {
		this->value = { key, value };
	}
	_nothrow void set(KeyT&& key, const ValueT& value) {
		this->value = { key, value };
	}
	_nothrow void set(const KeyT& key, ValueT&& value) {
		this->value = { key, value };
	}
	_nothrow void set(KeyT&& key, ValueT&& value) {
		this->value = { key, value };
	}
};

template <typename KeyT, typename ValueT, sentinel_converter_t<ValueT> Sentinel>
struct sentinel_associate_cache<KeyT, ValueT, 0, Sentinel> final {
	static _forceinline _nothrow bool is_sentinel(const ValueT & value) {
		return (decltype(Sentinel))(value) == Sentinel;
	}

	_forceinline _nothrow ValueT get(const KeyT& key) const {
		return ValueT(Sentinel);
	}

	_forceinline _nothrow ValueT get(KeyT&& key) const {
		return ValueT(Sentinel);
	}

	_forceinline _nothrow void set(const KeyT& key, const ValueT& value) {
	}
	_forceinline _nothrow void set(KeyT&& key, const ValueT& value) {
	}
	_forceinline _nothrow void set(const KeyT& key, ValueT&& value) {
	}
	_forceinline _nothrow void set(KeyT&& key, ValueT&& value) {
	}
};
