#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
using namespace std;

//ABRE Y SEPARA EL PARRAFO EN PALABRAS
vector <string> lecturaParrafo(string dir){
    string v;
    vector <string> palabras;
    ifstream f;
    f.open(dir);
    while(!f.eof()){
        getline(f, v); //toda la linea
        palabras.push_back(v);
        v="";
    }
    f.close();

    return palabras;
}

//IMPRIME VECTOR
void imprimirVector(vector <string> vectorsote){
    for(int i=0; i<vectorsote.size(); i++){
        cout<<vectorsote[i]<<" ";
    }
}

struct Produccion
{
    // Ej. aAb -> aBb + aAcb
    string first; // parte izquierda
    string second; // parte derecha
    Produccion(string f, string s){
        first = f;
        second = s;
    };
    Produccion(){
        first = "";
        second = "";
    };

    void imprimir();
};

void Produccion::imprimir(){
    cout<<"[f]"<<first<<" "<<"[s]"<<second;
}

class EarleyState
{
    Produccion produccion_actual; // ej. Verb -> Sust + Adj
    int pos_punto;
    int pos_chart;
    int estado_chart;
    int referencia_int; // profe dijo hacerlo puntero y entero
    EarleyState* referencia_ptr;
public:
    EarleyState(Produccion, int, int, int, int);
    friend class EarleyParser;
};

EarleyState::EarleyState(Produccion prod, int pp, int pc, int ec, int r)
{
    produccion_actual.first = prod.first;
    produccion_actual.second = prod.second;
    pos_punto = pp;     // pos del punto en la expresion
    pos_chart = pc;     // linea del chart actual
    estado_chart = ec;  // 0(inicio,predecir), 1(escanear,completar), 2(escanear,predecir), 3(... )
    referencia_int = r; // de donde viene la produccion actual
}

class EarleyParser
{
    vector<Produccion> P;   // vector de producciones
    vector<string> SNT;     // simbolos no terminales
    vector<string> ST;      // simbolos terminales
    string inicial;         // estado inicial
    string expresion;

    vector<EarleyState*> chart; // Chart es un arreglo de EarlyState
    // (...)
public:
    EarleyParser(vector <string>);
    void recibirEntrada(vector<string>);
    void imprimirChart();
    void predecir();
    void completar();
    void escanear();
};

void EarleyParser::recibirEntrada(vector<string> entrada)
{
    expresion = entrada[0];
    inicial = entrada[1];
    string lexema;
    for(int i=0; i<entrada[2].size(); i++){
        if(entrada[2][i]!=','){
            lexema.push_back(entrada[2][i]);
        }
        else{
            SNT.push_back(lexema);
            lexema.clear();
        }
    }
    SNT.push_back(lexema);
    lexema.clear();

    for(int i=0; i<entrada[3].size(); i++){
        if(entrada[3][i]!=','){
            lexema.push_back(entrada[3][i]);
        }
        else{
            ST.push_back(lexema);
            lexema.clear();
        }
    }
    ST.push_back(lexema);
    lexema.clear();

    int cant = atoi(entrada[4].c_str());
    int fin = 5 + cant;
    for(int i = 5; i < fin; i++){
        Produccion* tmp = new Produccion;
        for(int j=0; j<entrada[i].size(); j++){
            if(entrada[i][j]!= '='){
                lexema.push_back(entrada[i][j]);
            }
            else{
                tmp->first = lexema;
                lexema.clear();
            }
        }
        tmp->second = lexema;
        P.push_back(*tmp);
        lexema.clear();

    }

    cout<<"DATOS RECIBIDOS"<<endl;
    cout<<"Expresion      : "<<expresion<<endl;
    cout<<"Estado inicial : "<<inicial<<endl;
    cout<<"No Terminales  : ";
    imprimirVector(SNT);
    cout<<endl;
    cout<<"Terminales     : ";
    imprimirVector(ST);
    cout<<endl<<endl;
    cout<<"PRODUCCIONES"<<endl;
    for(int i=0; i<P.size(); i++){
        P[i].imprimir();
        cout << endl;
    }
    cout << endl;
}

void EarleyParser::imprimirChart()
{
    cout << "============== CHART ==============" << endl;
    cout << "pos_chart, produc_actual, pos_punto, estado, referencia\n\n";
    for(int i = 0; i < chart.size(); i++)
    {
        cout << chart[i]->pos_chart << ", ";
        chart[i]->produccion_actual.imprimir();
        cout << ", " << chart[i]->pos_punto << ", " << chart[i]->estado_chart << ", " << chart[i]->referencia_int << endl;
    }
}

void EarleyParser::predecir()
{

}

void EarleyParser::completar()
{

}

void EarleyParser::escanear()
{

}

/*
constructor:
{
    recibirEntrada();
    insertar primer estado en chart
    while()
    {
        predecir();
        escanear();
        completar();
        escanear();
    }
}
*/

EarleyParser::EarleyParser(vector <string> entrada)
{
    recibirEntrada(entrada);

    int pos_ch = 0;     // inicio pos_chart
    int estado_ch = 0;  // inicio estado_chart
    for(int i = 0; i < P.size(); i++)
    {
        if (P[i].first == inicial)
        {
            EarleyState* oEarley = new EarleyState(P[i], 0, pos_ch, estado_ch, 0);
            chart.push_back(oEarley);
            pos_ch++;
        }
    }

    // test
    imprimirChart();

}


// hacer el input - identificar la gramatica y la expresion a evaluar
int main()
{
    vector <string> entrada;
    entrada = lecturaParrafo("entrada.txt");

    EarleyParser earley(entrada);

    return 0;
}
