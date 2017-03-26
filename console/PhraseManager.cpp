#include <fstream>
#include "PhraseManager.h"



PhraseManager::PhraseManager()
{
}


PhraseManager::~PhraseManager()
{
}


bool PhraseManager::load(const std::string& rs)
{
    bool result = false;
    std::ifstream ifs;
    ifs.open(rs);

    if (ifs.is_open())
    {
        std::string s;
        while (std::getline(ifs, s))
        {
            if (s.length())
            {
                phrases.push_back(s);
            }
        }

        ifs.close();

        if (phrases.size())
        {
            next_phrase_index = 0;
        }
        
        result = true;
    }

    return result;
}


std::string PhraseManager::next_phrase(void)
{
    std::string s;
    if (phrases.size())
    {
        int n = next_phrase_index;
        
        if (false)
        {
            // random for next time
            ///@TODO -- add support for a random mode
        }
        else
        {
            // in sequence from file
            next_phrase_index = (next_phrase_index + 1) % phrases.size();
        }
        
        s = phrases[n];
    }
    else
    {
        s = "this is a test";
    }

    return s;
}
