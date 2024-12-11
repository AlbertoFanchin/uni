#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <fstream> // Per la gestione dei file
#include <cstdlib> // Per rand() e srand()
#include <ctime> // Per time()
#include <algorithm> // Per std::shuffle

// Definizione di una struttura per rappresentare un hotel
struct Hotel {
    // Numero di stanze
    int numeroStanze;

    // I corridoi sono rappresentati come un grafo orientato usando una mappa di adiacenza
    std::vector<std::vector<int>> corridoi;

    // Metodo per inizializzare l'hotel
    void inizializza(int stanze, int numeroCorridoi) {
        numeroStanze = stanze;
        corridoi.resize(numeroStanze + 1); // Indici da 1 a numeroStanze
    }

    // Metodo per aggiungere un corridoio (direzione singola)
    void aggiungiCorridoio(int da, int a) {
        if (da > 0 && da <= numeroStanze && a > 0 && a <= numeroStanze) {
            corridoi[da].push_back(a);
        } else {
            std::cerr << "Errore: una o entrambe le stanze non esistono!" << std::endl;
        }
    }

    // Trova e restituisce tutte le componenti fortemente connesse (SCC)
    std::vector<std::vector<int>> trovaSCC() {
        std::vector<int> indice(numeroStanze + 1, -1);
        std::vector<int> basso(numeroStanze + 1, -1);
        std::vector<bool> inStack(numeroStanze + 1, false);
        std::stack<int> stack;
        std::vector<std::vector<int>> sccs;
        int index = 0;

        for (int i = 1; i <= numeroStanze; ++i) {
            if (indice[i] == -1) {
                strongConnect(i, index, indice, basso, inStack, stack, sccs);
            }
        }

        return sccs;
    }

private:
    void strongConnect(int stanza, int& index, std::vector<int>& indice, std::vector<int>& basso,
                       std::vector<bool>& inStack, std::stack<int>& stack,
                       std::vector<std::vector<int>>& sccs) {
        indice[stanza] = basso[stanza] = index++;
        stack.push(stanza);
        inStack[stanza] = true;

        for (int vicina : corridoi[stanza]) {
            if (indice[vicina] == -1) {
                strongConnect(vicina, index, indice, basso, inStack, stack, sccs);
                basso[stanza] = std::min(basso[stanza], basso[vicina]);
            } else if (inStack[vicina]) {
                basso[stanza] = std::min(basso[stanza], indice[vicina]);
            }
        }

        if (basso[stanza] == indice[stanza]) {
            std::vector<int> componente;
            int v;
            do {
                v = stack.top();
                stack.pop();
                inStack[v] = false;
                componente.push_back(v);
            } while (v != stanza);

            if (componente.size() > 1 || (componente.size() == 1 && !corridoi[componente[0]].empty())) {
                sccs.push_back(componente);
            }
        }
    }
};

int main() {
    Hotel hotel;

    // Carica la struttura dell'hotel da un file
    std::string nomeFile = "input.txt"; // Nome del file da cui leggere il grafo
    std::ifstream file(nomeFile);
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file: " << nomeFile << std::endl;
        return 1;
    }

    int numeroStanze, numeroCorridoi;
    file >> numeroStanze >> numeroCorridoi;
    hotel.inizializza(numeroStanze, numeroCorridoi);

    for (int i = 0; i < numeroCorridoi; ++i) {
        int da, a;
        file >> da >> a;
        hotel.aggiungiCorridoio(da, a);
    }

    // Trova tutte le componenti fortemente connesse
    auto sccs = hotel.trovaSCC();

    if (sccs.empty()) {
        std::cout << "Non ci sono cicli infiniti nell'hotel." << std::endl;
        return 0;
    }

    // Determina una stanza universale, se esiste
    std::unordered_set<int> stanzeInCicli;
    for (const auto& scc : sccs) {
        stanzeInCicli.insert(scc.begin(), scc.end());
    }

    int stanzaUniversale = -1;
    for (int stanza : stanzeInCicli) {
        bool universale = true;
        for (const auto& scc : sccs) {
            if (std::find(scc.begin(), scc.end(), stanza) == scc.end()) {
                universale = false;
                break;
            }
        }
        if (universale) {
            stanzaUniversale = stanza;
            break;
        }
    }

    // Scrivi i risultati su un file
    std::ofstream outputFile("output.txt");
    if (!outputFile.is_open()) {
        std::cerr << "Errore nell'apertura del file di output." << std::endl;
        return 1;
    }

    if (stanzaUniversale != -1) {
        outputFile << stanzaUniversale << "\n";

        // Seleziona un ciclo casuale
        srand(static_cast<unsigned int>(time(nullptr)));
        int indiceCiclo = rand() % sccs.size();
        const auto& cicloScelto = sccs[indiceCiclo];

        outputFile << cicloScelto.size();
        for (int stanza : cicloScelto) {
            outputFile << " " << stanza;
        }
        outputFile << "\n";
    } else {
        outputFile << -1 << "\n";
    }

    outputFile.close();

    return 0;
}
