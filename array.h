#ifndef ARRAY_H_DEF
#define ARRAY_H_DEF

#include "types.h"


//
// Over-ridable definitions
//

#ifndef ARRAY_DEFAULT_INITIAL_SIZE
#define ARRAY_DEFAULT_INITIAL_SIZE 16
#endif

#ifndef _ARRAY_ALLOCATE_FUNC
#include <malloc.h>
#define _ARRAY_ALLOCATE_FUNC malloc
#endif

#ifndef _ARRAY_FREE_FUNC
#define _ARRAY_FREE_FUNC free
#endif

#ifndef _ARRAY_ASSERT_FUNC
#include <assert.h>
#define _ARRAY_ASSERT_FUNC assert
#endif

#ifndef _MEMCPY_FUNC
#include <string.h>
#define _MEMCPY_FUNC memcpy
#endif

#ifndef _MEMSET_FUNC
#include <string.h>
#define _MEMSET_FUNC memset
#endif


#define ARRAY_ALLOCATE _ARRAY_ALLOCATE_FUNC
#define ARRAY_ALLOCATE_TYPES(T, n) (_ARRAY_ALLOCATE_FUNC(sizeof(T) * (n)))
#define ARRAY_ALLOCATE_SIZE(size, n) (_ARRAY_ALLOCATE_FUNC((size)  * (n)))
#define ARRAY_FREE(p) (_ARRAY_FREE_FUNC(p))
#define ARRAY_ASSERT(exp) (_ARRAY_ASSERT_FUNC(exp))
#define ARRAY_MEMCPY _MEMCPY_FUNC
#define ARRAY_MEMSET _MEMSET_FUNC


namespace Array
{
  template <typename T>
  struct Array;

  // Array procedure prototypes
  // All take an Array<T> reference as their first argument
  //

  template <typename T>
  T *
  get(Array<T>& array, u32 index);

  template <typename T>
  s32
  get_element_index(Array<T>& array, T& element);

  template <typename T>
  void
  add_n(Array<T>& array, T *new_elements, u32 n_new_elements);

  template <typename T>
  void
  add(Array<T>& array, Array<T>& new_elements);

  template <typename T>
  void
  add(Array<T>& array, T new_element);

  template <typename T>
  void
  remove(Array<T>& array, u32 index);

  template <typename T>
  void
  remove_ordered(Array<T>& array, u32 remove_at_index);

  template <typename T>
  void
  free_array(Array<T>& array);

  template <typename T>
  void
  clear(Array<T>& array);

  template <typename T>
  void
  resize(Array<T>& array, u32 new_array_size);


  // Array<T> struct
  //
  // Array is a templated dynamic continuous array allocated on the heap.  The struct
  // includes an integral array so initial/small array(s) can be used without heap allocation.
  //
  template <typename T>
  struct Array
  {
    u32 array_size;  // Size of array pointed to by elements (in T units)
    u32 n_elements;  // Number of Ts in use in elements

    // Static storage for small number of elements, prevents heap allocation for small arrays
    T static_elements[ARRAY_DEFAULT_INITIAL_SIZE];

    T *elements;  // Pointer to array memory

    // Constructor:  Needed to test for - and do - initial heap allocation
    Array(u32 initial_size = ARRAY_DEFAULT_INITIAL_SIZE)
      : array_size(0),
        n_elements(0),
        elements(0)
    {
      resize(*this, initial_size);
    }

    // De-constructor: Free heap allocation
    ~Array()
    {
      free_array(*this);
    }

    // Convenience getter operator[]
    T &
    operator[](const u32 index)
    {
      return *get(*this, index);
    }
  };


  // Resize array memory to new_array_size*sizeof(T) and copy elements into the new memory.
  //
  // If new_array_size < n_elements, the array is truncated to the maximum number of elements that
  //   will fit in the new array.
  //
  // If new_array_size <= ARRAY_DEFAULT_INITIAL_SIZE, the static_elements array in Array is
  //   used instead to avoid heap allocations, for small short-lived arrays.
  //
  template <typename T>
  void
  resize(Array<T>& array, u32 new_array_size)
  {
    if (new_array_size == array.n_elements)
    {
      // No change
    }
    else
    {
      T *new_buffer = 0;
      if (new_array_size <= ARRAY_DEFAULT_INITIAL_SIZE)
      {
        // Use static struct array
        new_buffer = array.static_elements;
        new_array_size = ARRAY_DEFAULT_INITIAL_SIZE;
      }
      else
      {
        // Allocate array on heap
        new_buffer = (T *)ARRAY_ALLOCATE_TYPES(T, new_array_size);
      }

      ARRAY_ASSERT(new_buffer != 0);

      if (new_array_size > array.n_elements)
      {
        // Increasing size, don't need truncation
      }
      else
      if (new_array_size < array.n_elements)
      {
        // Truncate n_elements to fit in new_array_size
        array.n_elements = new_array_size;
      }

      // Don't copy if there wasn't a buffer previously allocated
      if (array.elements != 0)
      {
        ARRAY_MEMCPY(new_buffer, array.elements, array.n_elements*sizeof(T));
      }

      // Free the old elements buffer, unless we were using the static_element buffer
      if (array.elements != array.static_elements &&
          array.elements != 0)
      {
        ARRAY_FREE(array.elements);
      }
      array.elements = new_buffer;

      array.array_size = new_array_size;
    }
  }


  //
  // Getters
  //

  template <typename T>
  T *
  get(Array<T>& array, const u32 index)
  {
    ARRAY_ASSERT(index < array.n_elements);
    return array.elements + index;
  }

  // Searches the array for the index of the first element equal to the given element
  //
  template <typename T>
  s32
  get_element_index(Array<T>& array, T& element)
  {
    s32 result = -1;

    for (u32 test_index = 0;
         test_index < array.n_elements;
         ++test_index)
    {
      T& test_element = array[test_index];

      if (test_element == element)
      {
        result = test_index;
        break;
      }
    }

    return result;
  }


  // Unallocates the memory used by the array, and zeros n_elements
  //
  template <typename T>
  void
  free_array(Array<T>& array)
  {
    if (array.elements != array.static_elements &&
        array.elements != 0)
    {
      ARRAY_FREE(array.elements);
      array.array_size = 0;
    }
    array.elements = 0;
    array.n_elements = 0;
  }


  // Clears references to all elements in the array, but doesn't free the memory.  Useful if you need
  //   to delete all elements, but are likely to reuse the array and want to save the reallocation.
  //
  template <typename T>
  void
  clear(Array<T>& array)
  {
    array.n_elements = 0;
  }


  //
  // Element Adders
  //

  template <typename T>
  void
  add_n(Array<T>& array, T *new_elements, u32 n_new_elements)
  {
    u32 space_left = array.array_size - array.n_elements;
    if (space_left < n_new_elements)
    {
      resize(array, array.array_size * 2);
    }

    T *add_position = array.elements + array.n_elements;
    ARRAY_MEMCPY(add_position, new_elements, n_new_elements*sizeof(T));
    array.n_elements += n_new_elements;
  }

  template <typename T>
  void
  add(Array<T>& array, Array<T>& new_elements)
  {
    add_n(array, new_elements.elements, new_elements.n_elements);
  }

  template <typename T>
  void
  add(Array<T>& array, T new_element)
  {
    add_n(array, &new_element, 1);
  }


  //
  // Element Removers
  //

  // Unordered remove
  // Last element is moved into deleted element's position
  template <typename T>
  void
  remove(Array<T>& array, u32 index)
  {
    // Must get pointers before decrementing n_elements, so assertion in getter is not triggered
    T *to_remove = get(array, index);
    T *to_move = get(array, array.n_elements-1);

    array.n_elements -= 1;

    if (index != array.n_elements)
    {
      *to_remove = *to_move;
    }
  }

  // Ordered remove
  // All elements beyond deleted item are shifted up one place
  template <typename T>
  void
  remove_ordered(Array<T>& array, u32 remove_at_index)
  {
    for (u32 index = remove_at_index;
         index < array.n_elements - 1;
         ++index)
    {
      T *to_move = get(array, index+1);
      T *new_position = get(array, index);

      *new_position = *to_move;
    }

    array.n_elements -= 1;
  }

}  // Namespace Array


#endif

#ifdef ARRAY_DEMO
#include <stdio.h>

void
array_demo()
{
  Array::Array<u32> test_array;

  // Generate and print first 20 numbers in Fibonacci sequence
  Array::add(test_array, 0u);
  Array::add(test_array, 1u);
  for (u32 i = 0;
       i < 20-2;
       ++i)
  {
    u32 next = test_array[i] + test_array[i+1];
    Array::add(test_array, next);
  }
  printf("20 Fibonacci numbers: ");
  for (u32 i = 0;
       i < test_array.n_elements;
       ++i)
  {
    printf("%u ", test_array[i]);
  }
  printf("\n");


  // Remove (unordered) first 8 elements
  Array::Array<u32> unordered_removal_test;
  Array::add(unordered_removal_test, test_array);

  for (u32 i = 0;
       i < 8;
       ++i)
  {
    Array::remove(unordered_removal_test, i);
  }
  printf("Unordered removal of first 8 elements: ");
  for (u32 i = 0;
       i < unordered_removal_test.n_elements;
       ++i)
  {
    printf("%u ", unordered_removal_test[i]);
  }
  printf("\n");


  // Remove (ordered) first 8 elements
  Array::Array<u32> ordered_removal_test;
  Array::add(ordered_removal_test, test_array);

  for (u32 i = 0;
       i < 8;
       ++i)
  {
    Array::remove_ordered(ordered_removal_test, 0);
  }
  printf("Ordered removal of first 8 elements: ");
  for (u32 i = 0;
       i < ordered_removal_test.n_elements;
       ++i)
  {
    printf("%u ", ordered_removal_test[i]);
  }
  printf("\n");


  printf("Original sequence: ");
  for (u32 i = 0;
       i < test_array.n_elements;
       ++i)
  {
    printf("%u ", test_array[i]);
  }
  printf("\n");

}

#endif