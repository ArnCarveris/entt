#ifndef ENTT_ENTITY_SPARSE_RANGE_HPP
#define ENTT_ENTITY_SPARSE_RANGE_HPP

#include <unordered_map>

namespace entt {

	template<typename Entity>
	class SparseRange {
	public:
		using pos_type = Entity;
		using size_type = pos_type;
		using range_id_type = pos_type;
	private:
		using range_type = std::pair<pos_type, pos_type>;
		using ranges_type = std::unordered_map<range_id_type, range_type>;
		using range_it_type = typename ranges_type::iterator;
	public:
		/*! @brief Default constructor. */
		SparseRange() = default;

		/*! @brief Default destructor. */
		virtual ~SparseRange() = default;

		/*! @brief Copying a sparse range isn't allowed. */
		SparseRange(const SparseRange &) = delete;
		/*! @brief Default move constructor. */
		SparseRange(SparseRange &&) = default;

		/*! @brief Copying a sparse range isn't allowed. @return This sparse range. */
		SparseRange & operator=(const SparseRange &) = delete;
		/*! @brief Default move assignment operator. @return This sparse range. */
		SparseRange & operator=(SparseRange &&) = default;


		bool select(range_id_type id) noexcept {
			if (id > 0) {
				selected_identifier = id;
				selected_iterator = ranges.find(id);

				return selected_iterator != ranges.end();
			}
			else if (selected_identifier > 0) {
				selected_identifier = 0;

				if (selected_iterator == ranges.end()) {
					return false;
				}
			}
			else {
				selected_identifier = 0;
			}

			selected_iterator = ranges.end();

			return true;
		}
		bool assign(range_id_type id, pos_type first, pos_type last) noexcept {
			if (first > last) {
				return false;
			}

			ranges[id] = std::make_pair(first, last + 1);

			return true;
		}
		bool selected() const noexcept {
			return selected_identifier > 0 && selected_iterator != ranges.end();
		}
		bool deselect() noexcept {
			return select(0);
		}


		bool destroy(range_id_type id) noexcept {
			auto it = ranges.find(id);
			if (it == ranges.end())
			{
				return false;
			}

			ranges.erase(it);

			if (selected_identifier == id) {
				selected_identifier = 0;
				selected_iterator = ranges.end();
			}

			return true;
		}

		bool reset() noexcept {
			ranges.clear();
			selected_iterator = ranges.end();
			selected_identifier = 0;

			return true;
		}
		
		size_type size(range_id_type id) const noexcept {
			auto it = ranges.find(id);
			if (it != ranges.end()) {
				return it->second.second - it->second.first;
			}

			return 0;
		}

		size_type count() const noexcept {
			return ranges.size();
		}

		pos_type rbegin(pos_type unselected) const noexcept {
			if (selected_identifier > 0) {
				if (selected_iterator == ranges.end()) {
					return 0;
				}
				else {
					return selected_iterator->second.second;
				}
			}

			return unselected;
		}

		pos_type rend(pos_type unselected) const noexcept {
			if (selected_identifier > 0 && selected_iterator != ranges.end())
			{
				return selected_iterator->second.first;
			}

			return unselected;
		}

		bool expand(size_type size) noexcept {
			if (selected_identifier == 0) {
				return false;
			}

			if (selected_iterator == ranges.end()) {
				selected_iterator = ranges.emplace(selected_identifier, std::make_pair(size - 1, size)).first;
			}
			else {
				selected_iterator->second.second = size;
			}

			return true;
		}
	private:
		ranges_type		ranges{};
		range_it_type	selected_iterator{};
		range_id_type	selected_identifier{0};
	};
}


#endif // ENTT_ENTITY_SPARSE_RANGE_HPP