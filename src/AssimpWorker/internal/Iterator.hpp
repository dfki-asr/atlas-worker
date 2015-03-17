/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <functional>
#include <iterator>
#include <utility>
#include "Exception.hpp"

namespace AssimpWorker {

template<class Collection, typename OtherIterator>
class Iterator {
	typedef OtherIterator iterator_type;
	typedef decltype(*std::declval<iterator_type>()) value_type;

	public:
		Iterator(Collection& entirety) :
			end(std::end(entirety)),
			begin(std::begin(entirety))
		{
			return;
		}

		// This only works when Iterator == Collection::iterator
		// (because we're inferring the end with std::end)
		Iterator(Collection& entirety, iterator_type& begin) :
			end(std::end(entirety)),
			begin(begin)
		{
			return;
		}

		Iterator(Collection &entirety, std::function<bool(value_type)> filter)
		{
			throw NotYetSupportedException("filtering will come later");
		}

		Iterator(OtherIterator begin, OtherIterator end) :
			begin(begin), end(end)
		{
			return;
		}

		bool hasNext() {
			return begin == end;
		}
		value_type next() {
			if (!hasNext()) throw Exception("no more elements to return");
			return *begin++;
		}

	private:
		iterator_type end;
		iterator_type begin;
};

template <typename CollectionType>
static auto
makeIterator(CollectionType& c) -> Iterator<CollectionType, decltype(std::begin(c))>
{
	return Iterator<CollectionType, decltype(std::begin(c))>(c);
}

}
