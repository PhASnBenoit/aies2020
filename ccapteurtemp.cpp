#include <ccapteurtemp.h>

using namespace std;

CCapteurTemp::CCapteurTemp()
{

}

float CCapteurTemp::getTemp()
{
    QString mesure;
    float var;
    QDir dir;
    QString base=BASE;
    dir.cd(base);
    dir.setFilter(QDir::AllEntries);
    QFileInfoList list = dir.entryInfoList();
    for (int i=0 ; i<list.size() ; i++)
    {
         QFileInfo fileinfo = list.at(i);
         if (fileinfo.isSymLink())
         {
             if (fileinfo.fileName().left(3) == PREFIXE)
             {
                 base = fileinfo.symLinkTarget()+"/w1_slave";
                 QFile file(base);
                 if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                 {
                     qDebug() << "[CCapteur::getTemp]: Ouverture du fichier impossible";
                     return -99.99;
                 }
                 else
                 {
                     QTextStream in(&file);
                     while (!in.atEnd())
                     {
                         QString line = in.readLine();
                         if (line.contains("NO"))
                             return -99.99;
                         int pos;
                         if ( (pos=line.indexOf("t=")) > 1)
                         {
                             mesure = line.mid(pos+2,3);
                             var = mesure.toFloat()/10;
                             return var;
                         } // if temps
                     } // while
                 } // else open
             } // if prefixe
         } // if symbolic
    } // for
    return -99.99;
}
