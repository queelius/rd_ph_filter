#ifndef __HIDDEN_QUERY_H__
#define __HIDDEN_QUERY_H__

#include "picojson.h"
#include "Entropy.h"
#include <iostream>
#include "Hash.h"
#include "Utils.h"

namespace alex::encrypted_search
{
    class Trapdoor
    {
    public:

    private:
    };

    class HiddenQuery
    {
        

        void fromJson(const std::string& json)
        {
            std::stringstream ss;
            ss << json;
            picojson::value v;
            ss >> v;
            fromJson(v);
        };

        void fromJson(picojson::value v)
        {
            if (!v.is<picojson::object>())
                throw std::exception("Expected Object");
            const auto& x1 = v.get<picojson::object>();
            if (x1.count("hidden_query") == 0)
                throw std::exception("Expected \"hidden_query\" Key");
            const auto& x2 = x1.at("hidden_query");
            if (!x2.is<picojson::array>())
                throw std::exception("Expected Array");
            const auto& x3 = x2.get<picojson::array>();

            terms.clear();
            for (const auto& term : x3)
            {
                if (!term.is<picojson::array>())
                    throw std::exception("Expected Array 2");

                std::vector<std::string> tmp;
                for (const auto& e : term.get<picojson::array>())
                {
                    if (!e.is<std::string>())
                        throw std::exception("Expected String");
                    tmp.push_back(e.get<std::string>());
                }
                terms.push_back(tmp);
            }
        };

        picojson::value toJson() const
        {
            std::stringstream json;
            json << "{ \"hidden_query\": [";
            for (size_t i = 0; i < terms.size(); ++i)
            {
                if (i != 0)
                    json << ",";
                json << "[";
                for (size_t j = 0; j < terms[i].size(); ++j)
                {
                    if (j != 0)
                        json << ",";
                    json << "\"" << terms[i][j] << "\"";
                }
                json << "]";
            }
            json << "]}";

            picojson::value v;
            picojson::parse(v, json);
            return v;
        };

        std::string toJsonString() const
        {
            std::stringstream json;
            json << "{ \"hidden_query\": [";
            for (size_t i = 0; i < terms.size(); ++i)
            {
                if (i != 0)
                    json << ",";
                json << "[";
                for (size_t j = 0; j < terms[i].size(); ++j)
                {
                    if (j != 0)
                        json << ",";
                    json << "\"" << terms[i][j] << "\"";
                }
                json << "]";
            }
            json << "]}";

            return json.str();
        };

        HiddenTerm operator[](size_t idx) const
        {
            if (idx >= terms.size())
                throw std::exception("Invalid Index");

            return terms[idx];
        };

        uint32_t toHashCode() const
        {
            uint32_t hashCode = 0;
            for (const auto& term : terms)
                hashCode += toHashCode(term);
            return hashCode;
        };

        static uint32_t toHashCode(const HiddenTerm& term)
        {
            uint32_t hashCode = 0;
            for (const auto& w : term)
                hashCode += alex::hash::jenkinsHash(w);
            return hashCode;
        };

        std::vector<HiddenTerm> terms;
    };

    class HiddenQueryBuilder
    {
    public:
        struct BuilderParams
        {
            std::vector<std::string> secrets;
            int obfuscations;
            QueryBuilder qb;

            BuilderParams()
            {
                defaults();
            };

            void defaults()
            {
                qb._params.defaults();
                secrets = alex::globals::default_secrets;
                obfuscations = alex::globals::default_obfuscations;
            };
        };

        HiddenQueryBuilder(const BuilderParams& params) : _params(params) {};

        Query makePlainQuery()
        {
            return _params.qb.make();
        };

        void clear()
        {
            _hQuery.terms.clear();
        };

        HiddenQuery make(const std::string& q)
        {
            clear();
            _params.qb.clear();
            _params.qb.parse(q);
            _params.qb.normalize();
            Query qp = _params.qb.make();
            parse(qp);
            obfuscate(_params.obfuscations);
            normalize();
            return make();
        };

        // if a phrase like "go go go" is searched, both hidden biwords will have same hash UNLESS
        // multiple secrets are used.
        void parse(const Query& q)
        {
            for (const auto& t: q.terms)
            {
                //std::cout << alex::utils::toString(t) << std::endl;
                std::vector<std::string> term;
                if (t.size() == 1)
                {
                    // choose one of the secrets
                    term.push_back(alex::crypt::hexdigest<alex::globals::digest_size>(t[0] + "|" +
                        _params.secrets[_entropy.get() % _params.secrets.size()]));
                }
                else if (!t.empty())
                {
                    for (size_t i = 0; i < t.size() - 1; ++i)
                    {
                        term.push_back(alex::crypt::hexdigest<alex::globals::digest_size>(t[i] + " " + t[i+1] + "|" +
                            _params.secrets[_entropy.get() % _params.secrets.size()]));
                    }
                }
                _hQuery.terms.push_back(term);
            }
        };

        void normalize()
        {
            auto& ts = _hQuery.terms;
            std::sort(ts.begin(), ts.end());
            auto newEnd = std::unique(ts.begin(), ts.end());
            ts.erase(newEnd, ts.end());
        };

        void obfuscate(size_t n = 1)
        {
            static const std::vector<char> alphabet =
            {
                '0', '1', '2', '3',
                '4', '5', '6', '7',
                '8', '9', 'a', 'b',
                'c', 'd', 'e', 'f'
            };

            for (size_t i = 0; i < n; ++i)
            {
                std::string noise;
                noise.reserve(alex::globals::digest_size);
                for (unsigned int i = 0; i < alex::globals::digest_size; ++i)
                    noise.push_back(alphabet[_entropy.get() % alphabet.size()]);
                _hQuery.terms.push_back(std::vector<std::string>{noise});
            }
        };

        HiddenQuery make() const
        {
            return _hQuery;
        };

    private:
        HiddenQuery _hQuery;
        BuilderParams _params;
        stochastic::Entropy<std::minstd_rand0> _entropy;
    };
}

#endif
