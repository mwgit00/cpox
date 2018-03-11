#ifndef PHRASE_MANAGER_H_
#define PHRASE_MANAGER_H_

#include <string>
#include <vector>

class PhraseManager
{
public:

    typedef struct _T_phrase_info
    {
        std::string text;
        std::string wav;
    } T_phrase_info;
    
    PhraseManager();
    virtual ~PhraseManager();

    bool load(const std::string& rs);

    const T_phrase_info& next_phrase(void);

private:
    
    T_phrase_info default_phrase;
    std::vector<T_phrase_info> phrases;
    int next_phrase_index;
};

#endif // PHRASE_MANAGER_H_
