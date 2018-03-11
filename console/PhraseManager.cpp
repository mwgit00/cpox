#include <fstream>
#include <sstream>
#include <iostream>
#include <opencv2/core/core.hpp>
#include "PhraseManager.h"



PhraseManager::PhraseManager() :
    default_phrase({ "this is a test", "" }),
    phrases(),
    next_phrase_index(0)
{
}


PhraseManager::~PhraseManager()
{
}


bool PhraseManager::load(const std::string& rs)
{
    bool result = false;

    cv::FileStorage fs;
    fs.open(rs, cv::FileStorage::READ);

    if (fs.isOpened())
    {
        cv::FileNode n;

        n = fs["phrases"];
        if (n.isSeq())
        {
            cv::FileNodeIterator iter;
            for (iter = n.begin(); iter != n.end(); iter++)
            {
                cv::FileNode& rnn = *iter;
                phrases.push_back(T_phrase_info({ rnn["text"], rnn["wav"] }));
            }

            // sanity check
            result = true;
            for (const auto& r : phrases)
            {
                if (!r.text.length())
                {
                    result = false;
                    break;
                }
            }
        }
    }

    next_phrase_index = 0;
    return result;
}


const PhraseManager::T_phrase_info&  PhraseManager::next_phrase(void)
{
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
        
        return phrases[n];
    }
    else
    {
        return default_phrase;
    }
}
