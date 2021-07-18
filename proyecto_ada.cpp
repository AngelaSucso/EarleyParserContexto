#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
using namespace std;

vector<string> split(string cadena, char simbolo){
    vector<string> vec;
    string tmp;
    for(int i=0; i<cadena.size();i++){
        if(cadena[i]!=simbolo){
            tmp.push_back(cadena[i]);
        }else{
            vec.push_back(tmp);
            tmp.clear();
        }
    }
    vec.push_back(tmp);

    return vec;
}

//ABRE Y SEPARA EL PARRAFO EN PALABRAS
vector <string> lecturaParrafo(string dir)
{
    string v;
    vector <string> palabras;
    ifstream f;
    f.open(dir);
    while(!f.eof())
    {
        getline(f, v); //toda la linea
        palabras.push_back(v);
        v="";
    }
    f.close();

    return palabras;
}

//IMPRIME VECTOR
void imprimirVector(vector <string> vectorsote)
{
    for(int i=0; i<vectorsote.size(); i++)
    {
        cout<<vectorsote[i]<<" ";
    }
}

// TOKEN ================================================

class Token{
public:
    string etiqueta;
    string valor;
    Token();
    Token(string,string);
    void imprimirToken();
};

Token::Token(){
    etiqueta = "";
    valor = "";
}

Token::Token(string _etiqueta, string _valor){
    etiqueta = _etiqueta;
    valor = _valor;
}

void Token::imprimirToken(){
    cout<<etiqueta<<"="<<valor;
}

// NODO =================================================

class Nodo{
public:
    string etiqueta;
    vector<Token*> tokens;
    Nodo();
    Nodo(string);
    void insertarToken(string,string);
    void imprimirTokens();
    void imprimirNodo();

    Nodo& operator = (const Nodo &original);
};

Nodo::Nodo(){
    etiqueta = "";
}

Nodo::Nodo(string _etiqueta){
    etiqueta = _etiqueta;
}

void Nodo::insertarToken(string etiqueta, string valor)
{
    Token *token = new Token(etiqueta,valor);
    tokens.push_back(token);
}

void Nodo::imprimirTokens(){
    for(int i=0; i<tokens.size(); i++){
        tokens[i]->imprimirToken();
    }
}

void Nodo::imprimirNodo(){
    cout<<etiqueta;
    cout<<"[";
    imprimirTokens();
    cout<<"]";
}

Nodo& Nodo::operator = (const Nodo &original)
{
    etiqueta = original.etiqueta;
    for (int i=0; i<original.tokens.size(); i++)
        tokens.push_back(original.tokens[i]);
    return *this;
}

//==========================================================

class Produccion
{
 public:
    // Ej. aAb -> aBb + aAcb
    Nodo *first;            // parte izquierda
    vector<Nodo*> second;   // parte derecha
    Produccion(Nodo* f, vector<Nodo*> s)
    {
        first = f;
        for(int i=0; i<s.size(); i++){
            second.push_back(s[i]);
        }
    };
    Produccion(){};
    Produccion& operator = (Produccion &original);
    void imprimirProduccion();
};

void Produccion::imprimirProduccion(){
    first->imprimirNodo();
    cout<<"->";
    for(int i=0; i<second.size(); i++){
        second[i]->imprimirNodo();
    }
}

Produccion& Produccion::operator = (Produccion &original){
    first = original.first;
    for(int i=0; i<original.second.size(); i++){
        second.push_back(original.second[i]);
    }
    return *this;
}

class EarleyState
{
public:
    Produccion produccion_actual;   // ej. Verb -> Sust + Adj
    int pos_punto;
    int pos_chart;
    int estado_chart;
    int referencia_int;             // profe dijo hacerlo puntero y entero
    EarleyState* referencia_ptr;
    EarleyState(Produccion, int, int, int, int);
    void imprimirEarleyState();
    friend class EarleyParser;
    friend bool busquedaChar(string, vector <EarleyState*>);
    friend void imprimirVector(vector <EarleyState*>);
};

EarleyState::EarleyState(Produccion prod, int pp, int pc, int ec, int r)
{
    produccion_actual = prod;
    pos_punto = pp;     // pos del punto en la expresion
    pos_chart = pc;     // linea del chart actual
    estado_chart = ec;  // 0(inicio,predecir), 1(escanear,completar), 2(escanear,predecir), 3(... )
    referencia_int = r; // de donde viene la produccion actual
}

//IMPRIME VECTOR
void imprimirVector(vector <EarleyState*> vectorsote)
{
    for(int i=0; i<vectorsote.size(); i++)
    {
        cout<<vectorsote[i]->produccion_actual.first->etiqueta<<" ";
    }
}

void EarleyState::imprimirEarleyState(){
    cout << pos_chart << ", ";
    produccion_actual.imprimirProduccion();
    cout << ", " << pos_punto << ", " << estado_chart << ", " << referencia_int << endl;
}

class EarleyParser
{
    vector<Produccion> P;   // vector de producciones
    vector<string> SNT;     // simbolos no terminales
    vector<string> ST;      // simbolos terminales
    string inicial;         // estado inicial
    vector<string> expresion;
    int pos_ch;
    int estado_ch;

    vector<EarleyState*> chart; // Chart es un arreglo de EarlyState
    // (...)
public:
    EarleyParser(vector <string>);
    void recibirEntrada(vector<string>);
    void imprimirChart();
    void predecir();
    void completar();
    void escanear();
    bool evaluacion_final();
    Nodo* estructurarNodo(string);
};

EarleyParser::EarleyParser(vector <string> entrada)
{
    recibirEntrada(entrada);

    pos_ch = 0;         // inicio pos_chart
    estado_ch = 0;  // inicio estado_chart
    for(int i = 0; i < P.size(); i++)
    {
        if (P[i].first->etiqueta == inicial)
        {
            EarleyState* oEarley = new EarleyState(P[i], 0, pos_ch, estado_ch, 0);
            chart.push_back(oEarley);
            pos_ch++;
        }
    }

    while(true)
    {
        predecir();
        if(expresion.size() == 0)
        {
            if(evaluacion_final())
            {
                cout<<"\nSi pertenece a la gramatica."<<endl;
            }
            else
            {
                cout<<"\nNo pertenece a la gramatica."<<endl;
            }
            break;
        }
        escanear();
        completar();
        if(expresion.size() == 0)
        {
            imprimirChart();
            if(evaluacion_final())
            {
                cout<<"\nSi pertenece a la gramatica."<<endl;
            }
            else
            {
                cout<<"\nNo pertenece a la gramatica."<<endl;
            }
            break;
        }
        escanear();
    }

}

bool EarleyParser::evaluacion_final()
{
    for(int i=0; i<chart.size(); i++)
    {
        if(chart[i]->estado_chart == estado_ch-1)  //buscar en el ultimo estado
        {
            if(chart[i]->pos_punto == chart[i]->produccion_actual.second.size())  //verifica que pos-punto este al final de su produccion
            {
                if(chart[i]->referencia_int == 0)  //verifica que su staterefence sea 0
                {
                    return true;
                }
            }
        }
    }
    return false;
}


Nodo* EarleyParser::estructurarNodo(string strNodo){
    string etNodo;
    string strTokens;
    Nodo* nodito = new Nodo();

    int flag = 0;
    for(int i=0; i<strNodo.size(); i++){
        if(flag==0){
            etNodo.push_back(strNodo[i]);
            if(strNodo[i+1]=='['){
                i++;
                flag = 1;
                nodito->etiqueta=etNodo;
            }
        }
        else if(flag==1){
            strTokens.push_back(strNodo[i]);
            if(strNodo[i+1]==']'){
                break;
            }
        }
    }

    //en el caso de que sea un nodo terminal
    if(flag == 0){
        nodito->etiqueta=etNodo;
        return nodito;
    }

    string etToken;
    string valToken;
    vector<string> vecTokens;
    vecTokens = split(strTokens,' ');
    for(int i=0; i<vecTokens.size(); i++){
        flag = 0;
        for(int j=0; j<vecTokens[i].size(); j++){
            if(flag == 0){
                etToken.push_back(vecTokens[i][j]);
                if(vecTokens[i][j+1]=='='){
                    j++;
                    flag = 1;
                }
            }
            else if(flag == 1){
                valToken.push_back(vecTokens[i][j]);
                if(vecTokens[i][j+1]==']'){
                    break;
                }
            }
        }
        //cout<<valToken<<endl;
        nodito->insertarToken(etToken,valToken);
        etToken.clear();
        valToken.clear();
    }

    return nodito;
}

void EarleyParser::recibirEntrada(vector<string> entrada)
{
    // entrada guardar en expresion
    expresion = split(entrada[0],' ');
    inicial = entrada[1];

    string palabra;
    for(int i=0; i<entrada[2].size(); i++)
    {
        if(entrada[2][i]!=' ')
        {
            palabra.push_back(entrada[2][i]);
        }
        else
        {
            SNT.push_back(palabra);
            palabra.clear();
        }
    }
    SNT.push_back(palabra);
    palabra.clear();

    for(int i=0; i<entrada[3].size(); i++)
    {
        if(entrada[3][i]!=' ')
        {
            palabra.push_back(entrada[3][i]);
        }
        else
        {
            ST.push_back(palabra);
            palabra.clear();
        }
    }
    ST.push_back(palabra);
    palabra.clear();

    int cant = atoi(entrada[4].c_str());
    int fin = 5 + cant;
    vector<string> strSecond;
    for(int i = 5; i < fin; i++)
    {
        Produccion* tmp = new Produccion;
        for(int j=0; j<entrada[i].size(); j++)
        {
            if(entrada[i][j]!= ':')
            {
                palabra.push_back(entrada[i][j]);
            }
            else
            {
                tmp->first = estructurarNodo(palabra);
                //cout<<tmp->first->etiqueta<<endl;
                palabra.clear();
            }
        }
        strSecond = split(palabra,' ');
        Nodo* ptrSecond;
        for(int j=0; j<strSecond.size(); j++){
            ptrSecond = estructurarNodo(strSecond[j]);
            tmp->second.push_back(ptrSecond);

        }
        P.push_back(*tmp);
        palabra.clear();
        strSecond.clear();
    }

    cout<<"DATOS RECIBIDOS"<<endl;
    cout<<"Expresion      : ";
    imprimirVector(expresion);
    cout<<endl;
    cout<<"Estado inicial : "<<inicial<<endl;
    cout<<"No Terminales  : ";
    imprimirVector(SNT);
    cout<<endl;
    cout<<"Terminales     : ";
    imprimirVector(ST);
    cout<<endl<<endl;
    cout<<"PRODUCCIONES"<<endl;
    for(int i=0; i<P.size(); i++)
    {
        P[i].imprimirProduccion();
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
        chart[i]->produccion_actual.imprimirProduccion();
        cout << ", " << chart[i]->pos_punto << ", " << chart[i]->estado_chart << ", " << chart[i]->referencia_int << endl;
    }
}

bool busquedaString(string letra, vector <string> vec)
{
    for(int i=0; i<vec.size(); i++)
    {
        if(vec[i] == letra)
        {
            return true;
        }
    }
    return false;
}

bool busquedaString(string letra, vector <EarleyState*> vec)
{
    for(int i=0; i<vec.size(); i++)
    {
        if(vec[i]->produccion_actual.first->etiqueta == letra)
        {
            return true;
        }
    }
    return false;
}

string char_to_string(char A)
{
    string tmp;
    tmp.push_back(A);
    return tmp;
}

void EarleyParser::predecir()
{
    vector <string> vec;
    int pos = chart[chart.size()-1]->pos_punto;
    string tmp;
    tmp = chart[chart.size()-1]->produccion_actual.second[pos]->etiqueta;

    vec.push_back(tmp);
    int pos_vec=0;

    while(true)  // ITERA POR EL VEC
    {
        for(int i=0; i<P.size(); i++)  // ITERA POR LAS PRODUCCIONES DE LA GRAMATICA
        {
            if(P[i].first->etiqueta == vec[pos_vec])   // COMPARA LA LETRA EN LA POS_PUNTO CON LA GRAMATICA
            {
                EarleyState* oEarley = new EarleyState(P[i], 0, pos_ch, estado_ch, estado_ch); //1° estado_ch = estado actual; 2° = ref
                chart.push_back(oEarley);
                pos_ch++;
                if(!busquedaString(P[i].second[0]->etiqueta, vec))
                {
                    vec.push_back(P[i].second[0]->etiqueta);
                }
            }
        }
        pos_vec++;
        if(pos_vec == vec.size())
            break;
    }

    estado_ch++;
}

void EarleyParser::escanear()
{
    string palabraTest = expresion[0];

    for(int i=0; i<chart.size(); i++)
    {
        if(chart[i]->estado_chart == estado_ch-1)
        {
            int pos = chart[i]->pos_punto;
            if(pos!=chart[i]->produccion_actual.second.size())
            {
                string tmp = chart[i]->produccion_actual.second[pos]->etiqueta;
                if(palabraTest == tmp)
                {
                    EarleyState* oEarley = new EarleyState(chart[i]->produccion_actual,
                        pos + 1, pos_ch, estado_ch, chart[i]->referencia_int);
                    chart.push_back(oEarley);
                    pos_ch++;
                }
            }
        }
    }
    expresion.erase(expresion.begin());
}

void EarleyParser::completar()
{
    vector <EarleyState*> vec;
    vec.push_back(chart[chart.size()-1]);

    int pos_vec = 0;
    int var_ref;
    string first_busqueda; // extrae palabra del vec

    while(true)  // itera por el vector
    {
        first_busqueda = vec[pos_vec]->produccion_actual.first->etiqueta;
        var_ref        = vec[pos_vec]->referencia_int;

        for(int i = 0; i < chart.size(); i++) //itera por el chart
        {
            int pos_tmp = chart[i]->pos_punto;

            if(pos_tmp!= chart[i]->produccion_actual.second.size())
            {
                string palabra_actual = chart[i]->produccion_actual.second[pos_tmp]->etiqueta;

                if(chart[i]->estado_chart == var_ref && palabra_actual == first_busqueda)
                {
                    EarleyState* oEarley = new EarleyState(chart[i]->produccion_actual,
                        pos_tmp + 1, pos_ch, estado_ch, chart[i]->referencia_int);
                    chart.push_back(oEarley);
                    pos_ch++;

                    if( !busquedaString(chart[i]->produccion_actual.first->etiqueta, vec) )
                    {
                        vec.push_back(chart[i]);
                    }
                }
            }
        }
        pos_vec++;

        if(pos_vec == vec.size())
            break;

    }
    estado_ch++;
}


int main()
{
    vector <string> entrada;
    entrada = lecturaParrafo("entrada.txt");

    EarleyParser earley(entrada);

    return 0;
}
