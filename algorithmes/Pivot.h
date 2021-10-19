#include <PosAtt.h>

#ifndef PIVOT_H
#define PIVOT_H

namespace algorithms
{
/**
 * @class Pivot
 * @brief Implémente l'algorithme du Pivot.
 */
class Pivot
{
public:
    //Tsw: liste de transformation Monde ------> Sensor ie S=Tsw W
    Pivot(PosAttList Tsw);

    //Calcul du pivot
    void computeSVD();

    //Retourne le pivot dans le referenciel Monde
    Eigen::Vector3d getPivot_W();

    //Retourne le pivot dans le referenciel Sensor
    Eigen::Vector3d getPivot_S();

    //Retourne une liste des erreurs de projection de Pivot_Sensor sur Pivot_Monde
    PosList getErr();

    //RMS Distances
    double getRMS();

private:
    PosAttList Tsw;
    Eigen::Vector3d pivot_W;
    Eigen::Vector3d pivot_S;
};

}
#endif // PIVOT_H

