#pragma once
#include "./curve.h"

class Bezier :
    public Curva
{
public:
    Bezier();
    void gerarCurva(int pontosPorSegmento);
};
