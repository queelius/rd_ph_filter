#ifndef __ISECURE_INDEX_H__
#define __ISECURE_INDEX_H__

#include <vector>
#include <string>
#include <iostream>

namespace alex::encrypted_search
{
    /*
     * A trapdoor is the result of a one-way transformation from x to t.
     * That is, going from x to t is 'easy' but going from t to x is 'hard'.
     */
    template <typename T>
    class Trapdoor
    {
    public:

    private:
    };

    template <typename T>
    using TrapdoorSequence = std::vector<Trapdoor<T>>;

    template <typename T>
    class TrapdoorGenerator
    {
    public:
        template <typename Iter>
        TrapdoorSequence<T> generate(Iter begin, Iter end)
        {
        }
                
        Trapdoor<T> generate(

    private:
    };

    template <bool Contains, bool Location, bool Freq, bool Sequences>
    class ISecureIndex
    {
    public:
        typedef uint32_t size_type;

        virtual void read(std::istream& file) = 0;
        virtual void write(std::ostream& file) = 0;

        virtual std::string get_id() const = 0;
        virtual void set_id(const std::string& id) = 0;

        virtual ISecureIndex* clone() const = 0;
        virtual void erase() = 0;
        virtual std::string preferred_file_extension() const = 0;

        virtual std::enable_if<Contains, bool> contains(const Trapdoor& trapdoor) const = 0;
        virtual std::enable_if<Contains && Sequences, std::vector<size_type>> contains(const TrapdoorSequence& trapdoors) const = 0;

        virtual std::enable_if<Location, std::vector<size_type>> locations(const Trapdoor& trapdoor) const = 0;
        virtual std::enable_if<Location && Sequences, std::vector<size_type>> locations(const TrapdoorSequence& trapdoors) const = 0;

        virtual std::enable_if<Freq, size_type> frequency(const Trapdoor& trapdoor) const = 0;
        virtual std::enable_if<Freq && Sequences, size_type> frequency(const TrapdoorSequences& trapdoors) const = 0;

        virtual float score(const HiddenQuery& query) const = 0;
    };
}

#endif