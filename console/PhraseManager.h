#ifndef PHRASE_MANAGER_H_
#define PHRASE_MANAGER_H_

#include <string>
#include <vector>

class PhraseManager
{
public:
    PhraseManager();
    virtual ~PhraseManager();

    bool load(const std::string& rs);
    std::string next_phrase(void);

private:
    
    std::vector<std::string> phrases;
    int next_phrase_index;
};

#endif // PHRASE_MANAGER_H_
