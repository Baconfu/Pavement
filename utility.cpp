#include "utility.h"

Utility::Utility()
{

}

QString Utility::trimString(QString s, QString target)
{
    while(s[s.length()-1] == target){
        s.remove(s.length()-1,1);
    }
    return s;
}
