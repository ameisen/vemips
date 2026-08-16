#pragma once

#if DEF_NEW_DELETE
# include <new>
#endif

#if WITH_CUSTOM_MALLOC
# include <cstdlib>
#endif

namespace musl {
#if WITH_CUSTOM_MALLOC
	void mfree(void *p);
	void* mrealloc(void *p, size_t n);
	void *mmalloc(size_t n);
	void *maligned_alloc(size_t align, size_t len);
#else
	void mfree(void *p) {
    ::free(p);
	}
	void* mrealloc(void *p, size_t n) {
    return ::realloc(p, n);
	}
	void *mmalloc(size_t n) {
	  return ::malloc(n);
	}
	void *maligned_alloc(size_t align, size_t len) {
	  return ::aligned_alloc(align, len);
	}
#endif
}

#if DEF_NEW_DELETE
void* operator new(size_t size)
{
  return musl::mmalloc(size);
}

void* operator new(size_t size, std::align_val_t align)
{
  return musl::maligned_alloc(size_t(align), size);
}

void* operator new[](size_t size)
{
  return musl::mmalloc(size);
}

void* operator new[](size_t size, std::align_val_t align)
{
  return musl::maligned_alloc(size_t(align), size);
}

void* operator new(size_t size, const std::nothrow_t& tag) noexcept
{
  return musl::mmalloc(size);
}

void* operator new(size_t size, std::align_val_t align, const std::nothrow_t& tag) noexcept
{
  return musl::maligned_alloc(size_t(align), size);
}

void* operator new[](size_t size, const std::nothrow_t& tag) noexcept
{
  return musl::mmalloc(size);
}

void* operator new[](size_t size, std::align_val_t align, const std::nothrow_t& tag) noexcept
{
  return musl::maligned_alloc(size_t(align), size);
}

void operator delete(void* ptr) noexcept
{
  musl::mfree(ptr);
}

void operator delete[](void* ptr) noexcept
{
  musl::mfree(ptr);
}

void operator delete(void* ptr, [[maybe_unused]] std::align_val_t align) noexcept
{
  musl::mfree(ptr);
}

void operator delete[](void* ptr, [[maybe_unused]] std::align_val_t align) noexcept
{
  musl::mfree(ptr);
}

namespace musl
{
  template <typename T>
  struct allocator {
    using value_type = T;

    allocator() noexcept = default;
    template <typename U>
    allocator(const allocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
      if (auto* p = musl::mmalloc(n * sizeof(T)))
        return static_cast<T*>(p);
      throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t n) noexcept {
      musl::mfree(p);
    }

    bool operator==(const allocator&) const noexcept { return true; }
    bool operator!=(const allocator&) const noexcept { return false; }
  };
}
#endif
