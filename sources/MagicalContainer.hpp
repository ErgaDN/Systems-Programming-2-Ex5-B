#ifndef MAGICALCONTAINER_HPP
#define MAGICALCONTAINER_HPP
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>

using namespace std;

namespace ariel
{
    class MagicalContainer
    {
        vector<int> _elements;
        vector<int *> _prime;

    public:
        MagicalContainer() {}
        // Disable copy constructor
        MagicalContainer(const MagicalContainer &) = delete;

        // Disable copy assignment operator
        MagicalContainer &operator=(const MagicalContainer &) = delete;
        // Disable move constructor
        MagicalContainer(MagicalContainer &&) = delete;

        // Disable move assignment operator
        MagicalContainer &operator=(MagicalContainer &&) = delete;
        ~MagicalContainer()
        {
            // Release memory for the int* pointers in _prime vector
            for (auto *ptr : _prime)
            {
                delete ptr;
            }
        }

        void addElement(int element);

        void removeElement(int element);

        size_t size() const
        {
            return _elements.size();
        }

        vector<int> &getVec() { return _elements; }
        vector<int *> &getPrime() { return _prime; }

        static bool isPrime(int number);

        class iterator
        {
        private:
            MagicalContainer &_container;
            size_t _index;
            bool _beginSide; // True for begin() side, False for end() side

        public:
            iterator(MagicalContainer &container) : _container(container), _index(0), _beginSide(true) {}
            iterator(iterator &other) : _container(other.getContainer()), _index(other.getIndex()), _beginSide(other.getBeginSide()) {}
            virtual ~iterator() = default;
            // Disable move constructor
            iterator(iterator &&) = delete;

            // Disable move assignment operator
            iterator &operator=(iterator &&) = delete;

            MagicalContainer &getContainer() { return _container; }
            size_t getIndex() const { return _index; }
            bool getBeginSide() const { return _beginSide; }

            void setIndex(size_t idx) { _index = idx; }
            void setBeginSide(bool boolean) { _beginSide = boolean; }

            bool operator==(const iterator &other) const;

            iterator &operator=(const iterator &other);

            bool operator!=(const iterator &other) const;

            bool operator>(const iterator &other) const;

            bool operator<(const iterator &other) const;

            virtual int operator*() = 0;

            virtual iterator &operator++() = 0;

            virtual iterator &begin() = 0;

            virtual iterator &end() = 0;
        };

        class AscendingIterator : public iterator
        {
        public:
            AscendingIterator(MagicalContainer &container) : iterator(container) {}

            int operator*() override;

            AscendingIterator &operator++() override;

            AscendingIterator &begin() override;

            AscendingIterator &end() override;
        };

        class SideCrossIterator : public iterator
        {
        public:
            SideCrossIterator(MagicalContainer &container) : iterator(container) {}

            int operator*() override;

            SideCrossIterator &operator++() override;

            SideCrossIterator &begin() override;

            SideCrossIterator &end() override;
        };

        class PrimeIterator : public iterator
        {
        public:
            PrimeIterator(MagicalContainer &container) : iterator(container) {}

            int operator*() override;

            PrimeIterator &operator++() override;

            PrimeIterator &begin() override;

            PrimeIterator &end() override;
        };
    };
}
#endif