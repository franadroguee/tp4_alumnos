#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>

#include "graphClass.h"

using namespace std;

// Vértices: nombre de la ciudad, con su provincia como dato.
// Aristas: distancia aproximada en km. Cada ruta está en los dos sentidos.
using GrafoCiudades = Graph<string, int>;

// ================================================================
//                     CARGA DEL GRAFO (ya dada)
// ================================================================

static string strip(const string& s) {
    const char* ws = " \t\r\n";
    size_t b = s.find_first_not_of(ws);
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(ws);
    return s.substr(b, e - b + 1);
}

static vector<string> split(const string& l) {
    vector<string> campos;
    stringstream ss(l);
    string campo;
    while (getline(ss, campo, ',')) campos.push_back(strip(campo));
    return campos;
}

static bool cargarCiudades(GrafoCiudades& g, const string& path) {
    ifstream file(path);
    if (!file) {
        cerr << "No se pudo abrir el archivo: " << path << "\n";
        return false;
    }
    string l;
    getline(file, l);  // encabezado: ciudad,provincia,lat,lon
    while (getline(file, l)) {
        l = strip(l);
        if (l.empty()) continue;
        vector<string> c = split(l);
        g.add_vertex(c[0], c[1]);
    }
    return true;
}

static bool cargarRutas(GrafoCiudades& g, const string& path) {
    ifstream file(path);
    if (!file) {
        cerr << "No se pudo abrir el archivo: " << path << "\n";
        return false;
    }
    string l;
    getline(file, l);  // encabezado: ciudad1,ciudad2,distancia_km
    while (getline(file, l)) {
        l = strip(l);
        if (l.empty()) continue;
        vector<string> c = split(l);
        int distancia = stoi(c[2]);
        g.add_edge(c[0], c[1], distancia);
        g.add_edge(c[1], c[0], distancia);
    }
    return true;
}

// Posición de la ciudad en g.get_vertices() (-1 si no está).
int indiceDe(const GrafoCiudades& g, const string& ciudad) {
    vector<string> ciudades = g.get_vertices();
    for (int i = 0; i < (int)ciudades.size(); i++) {
        if (ciudades[i] == ciudad) return i;
    }
    return -1;
}

// ================================================================
//                  EJERCICIO 1: COMPONENTES CONEXAS
// ================================================================

//FUNCION AYUDA; Es como indiceDe pero no copia el vector en cada llamado. Ahorra O(n) en memoria
static int buscar(const vector<string>& ciudades, const string& c) {
    for (int i = 0; i < (int)ciudades.size(); i++) {
        if (ciudades[i] == c) return i;
    }
    return -1;
}

vector<vector<string>> componentesConexas(const GrafoCiudades& g) {
    // TODO: implementar
    vector<vector<string>> componentes;
    vector<string> ciudades = g.get_vertices();
    vector<bool> visitados(ciudades.size(), false);

    for (int i = 0; i < (int)visitados.size(); i++) {
        if (!visitados[i]) {
            //Hacemos un BSF por cada ciudad que no visitamos, marcando todas las ciudades visitadas.
            queue<string> cola;
            vector<string> componente;
            cola.push(ciudades[i]);
            visitados[i] = true;

            while (!cola.empty()) {
                string ciudad = cola.front();
                cola.pop();
                componente.push_back(ciudad);

                for (string vecino : g.get_neighbors(ciudad)) {
                    int ind = buscar(ciudades, vecino);
                    if (!visitados[ind]) {
                        cola.push(vecino);
                        visitados[ind] = true;
                    }
                }
            }
            componentes.push_back(componente);
        }
    }
    return componentes;
}

// ================================================================
//        EJERCICIO 2: CAMINO MÍNIMO CON PESOS DE TODOS CON TODOS
// ================================================================

const int INFINITO = 1000000000;


// dist[i][j]: distancia mínima en km entre la ciudad i y la ciudad j, con
// las ciudades numeradas en el orden de g.get_vertices() (INFINITO si no hay
// camino).
vector<vector<int>> distanciasTodosConTodos(const GrafoCiudades& g) {
    // TODO: implementar
    vector<string> ciudades = g.get_vertices();
    int n = (int)ciudades.size();
    vector<vector<int>> vec(n), distancia(n);
    for (int i = 0; i < n; i++) {
        for (string& vecino : g.get_neighbors(ciudades[i])) {
            vec[i].push_back(buscar(ciudades, vecino));
            distancia[i].push_back(g.get_edge_data(ciudades[i], vecino).value());
        }
    }

    vector<vector<int>> dist(n, vector<int>(n, INFINITO));

    for (int i = 0; i < n; i++) {
        vector<int>& dist_d_actual = dist[i];
        vector<bool> cerrado(n, false);
        dist_d_actual[i] = 0;
        for (int j = 0; j < n; j++) {
            int u = -1;
            for (int k = 0; k < n; k++) {
                if (!cerrado[k] && dist_d_actual[k] != INFINITO && (u == -1 || dist_d_actual[k] < dist_d_actual[u])) {
                    u = k;
                }
            }

            if (u == -1) {break;}
            cerrado[u] = true;
            
            for (int s = 0; s < (int)vec[u].size(); s++) {
                int v = vec[u][s];
                if (dist_d_actual[u] + distancia[u][s] < dist_d_actual[v]) {
                    dist_d_actual[v] = dist_d_actual[u] + distancia[u][s];
                }
            }
        }
    }
    return dist;
}

// ================================================================
//                     EJERCICIO 3: TRIÁNGULOS
// ================================================================

int contarTriangulos(const GrafoCiudades& g) {
    // TODO: implementar
    return 0;
}

// ================================================================
//                EJERCICIO 4: DIÁMETRO CON PESOS
// ================================================================

class Diametro {
public:
    int distancia;
    string ciudad1;
    string ciudad2;
};

Diametro diametro(const GrafoCiudades& g, const vector<vector<int>>& dist) {
    // TODO: implementar
    return {0, "", ""};
}

// ================================================================
//          EJERCICIO 5: RANDOM WALKS Y LA CIUDAD MÁS CENTRAL
// ================================================================

// visitas[c]: cantidad de veces que los random walks pasaron por c.
using Visitas = unordered_map<string, int>;

Visitas randomWalks(const GrafoCiudades& g, int cantidad, int largo) {
    // TODO: implementar
    vector<string> ciudades = g.get_vertices();
    int n = (int)ciudades.size();
    Visitas visitas;
    
    vector<vector<int>> vecinos(n);

    for (int i = 0; i < n; i++) {
        for (string& v : g.get_neighbors(ciudades[i])){
            vecinos[i].push_back(buscar(ciudades, v));
        }
    }

    mt19937 gen(42);
    uniform_int_distribution<int> inicio(0, n - 1);
    vector<int> cont(n, 0);

    for (int i = 0; i < cantidad; i++) {
        int actual = inicio(gen);
        cont[actual] ++; 

        for (int paso = 0; paso < largo; paso++) {
            if (vecinos[actual].empty()) {break;}
            uniform_int_distribution<int> elegir(0, (int)vecinos[actual].size() - 1);
            int k = elegir(gen);
            actual = vecinos[actual][k];
            cont[actual] ++;
        }
    }

    for (int i = 0; i < n; i++) {
        visitas[ciudades[i]] = cont[i];
    }

    return visitas;
}

Visitas randomWalksPorDistancia(const GrafoCiudades& g, int cantidad, int largo) {
    // TODO: implementar
        vector<string> ciudades = g.get_vertices();
    int n = (int)ciudades.size();
    Visitas visitas;
    
    vector<vector<int>> vecinos(n);
    vector<discrete_distribution<int>> prob(n);

    for (int i = 0; i < n; i++) {
        vector<double> pesos;
        for (string& v : g.get_neighbors(ciudades[i])){
            vecinos[i].push_back(buscar(ciudades, v));
            pesos.push_back(1.0 / (double)g.get_edge_data(ciudades[i], v).value());
        }
        if (!pesos.empty()) {
            prob[i] = discrete_distribution<int>(pesos.begin(), pesos.end());
        }
    }

    mt19937 gen(42);
    uniform_int_distribution<int> inicio(0, n - 1);
    vector<int> cont(n, 0);

    for (int i = 0; i < cantidad; i++) {
        int actual = inicio(gen);
        cont[actual] ++; 

        for (int paso = 0; paso < largo; paso++) {
            if (vecinos[actual].empty()) {break;}
            uniform_int_distribution<int> elegir(0, (int)vecinos[actual].size() - 1);
            int k = prob[actual](gen);
            actual = vecinos[actual][k];
            cont[actual] ++;
        }
    }
    
    for (int i = 0; i < n; i++) {
        visitas[ciudades[i]] = cont[i];
    }

    return visitas;

}

static void imprimirTop(const vector<string>& nombres, const vector<int>& valores, int k) {
    vector<bool> usado(nombres.size(), false);
    for (int t = 0; t < k; t++) {
        int m = -1;
        for (int i = 0; i < (int)nombres.size(); i++)
            if (!usado[i] && (m == -1 || valores[i] > valores[m])) m = i;
        if (m == -1) break;
        usado[m] = true;
        cout << "  " << t + 1 << ". " << nombres[m] << " (" << valores[m] << ")\n";
    }
}

// ================================================================
//                   EJERCICIO 6: ANÁLISIS LIBRE
// ================================================================

void analisisLibre(const GrafoCiudades& g) {
    // TODO: implementar
    return;
}

// ================================================================
//                              MAIN
// ================================================================

int main(int argc, char* argv[]) {
    string pathCiudades = argc > 1 ? argv[1] : "ciudades_argentina.csv";
    string pathRutas = argc > 2 ? argv[2] : "grafo_ciudades.csv";

    GrafoCiudades g;
    if (!cargarCiudades(g, pathCiudades)) return 1;
    if (!cargarRutas(g, pathRutas)) return 1;
    cout << "Ciudades cargadas: " << g.get_vertices().size() << "\n\n";

    cout << "=== Ejercicio 1 ===\n";
    vector<vector<string>> componentes = componentesConexas(g);
    cout << "Componentes conexas: " << componentes.size() << "\n";
    // TODO: tamaño de la más grande y ciudades que quedan fuera de ella

    cout << "\n=== Ejercicio 2 ===\n";
    vector<vector<int>> dist = distanciasTodosConTodos(g);
    // TODO: distancias Buenos Aires - La Quiaca y Buenos Aires - Ushuaia
    int ba = indiceDe(g, "Buenos Aires");
    int lq = indiceDe(g, "La Quiaca");
    int us = indiceDe(g, "Ushuaia");

    cout << "Buenos Aires - La Quiaca: ";
    if (dist[ba][lq] == INFINITO) cout << "sin camino\n";
    else cout << dist[ba][lq] << " km\n";

    cout << "Buenos Aires - Ushuaia: ";
    if (dist[ba][us] == INFINITO) cout << "sin camino\n";
    else cout << dist[ba][us] << " km\n";

    cout << "\n=== Ejercicio 3 ===\n";
    cout << "Triángulos: " << contarTriangulos(g) << "\n";

    cout << "\n=== Ejercicio 4 ===\n";
    Diametro d = diametro(g, dist);
    cout << "Diámetro: " << d.distancia << " km, entre " << d.ciudad1 << " y "
              << d.ciudad2 << "\n";

    cout << "\n=== Ejercicio 5 ===\n";
    Visitas visitas = randomWalks(g, 10000, 100);
    Visitas visitasPorDistancia = randomWalksPorDistancia(g, 10000, 100);
    // TODO: las 10 ciudades más visitadas en cada caso, y las 10 de mayor grado

    vector<string> nombres;
    vector<int> v1, v2, grados;

    for (const string& c : g.get_vertices()) {
        nombres.push_back(c);
        v1.push_back(visitas[c]);
        v2.push_back(visitasPorDistancia[c]);
        grados.push_back((int)g.get_neighbors(c).size());
    }

    cout << "\nTop 10 (vecino al azar):\n\n";        imprimirTop(nombres, v1, 10);
    cout << "\nTop 10 (probabilidad ~ 1/km):\n\n";   imprimirTop(nombres, v2, 10);
    cout << "\nTop 10 por grado:\n\n";               imprimirTop(nombres, grados, 10);

    cout << "\n=== Ejercicio 6 ===\n";
    analisisLibre(g);

    return 0;
}
