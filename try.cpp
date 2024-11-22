#include <iostream>
#include <vector> // Contenedor vector
#include <string> // Trabajar con cadenas de texto
#include <cstdlib> // Funciones de generación random
#include <ctime> // Permite inicializar rand() con el tiempo actual
#include <sstream> // Manipular cadenas como stringstream
#include <algorithm> // Para el remove_if()

using namespace std; // Sin prefijar elementos con std

// Clase Jugador
class Jugador {
private: // Atributos
    string tipo;           // "rápido" o "lento"
    string experiencia;    // "novato" o "experto"
    string estado;         // Estado del jugador
    int x, y;              // Posición en el tablero
    char equipo;           // 'X' o 'O'
    int id;                // Identificador del jugador
    bool eliminado;        // Si el jugador ha sido eliminado
    int impactosExtremidades; // Contador de impactos en extremidades

public: // Constructor
    Jugador(string tipo, string experiencia, char equipo, int id, int x, int y)
        : tipo(tipo), experiencia(experiencia), equipo(equipo), id(id), x(x), y(y),
          eliminado(false), estado("intacto"), impactosExtremidades(0) {}  // Lista de inicialización
    // Métodos de clase
    bool mover(vector<string> direcciones, class Tablero& tablero); // Objeto de la clase Tablero
    bool atacar(int objetivo_x, int objetivo_y, class Tablero& tablero); // & pasar el objeto por referencia y no por copia
    void recibirImpacto(string parte, class Tablero& tablero, Jugador& atacante);
    bool estaEliminado() { return eliminado; }
    void mostrarEstado();
    // Getters
    int getX() { return x; }
    int getY() { return y; }
    char getEquipo() { return equipo; }
    int getId() { return id; }
    string getTipo() { return tipo; }
    string getExperiencia() { return experiencia; }
    string getEstado() { return estado; }
    int getImpactosExtremidades() { return impactosExtremidades; }
    void setPosicion(int x, int y) { this->x = x; this->y = y; } // Cambiar la posición del jugador en el tablero
};

// Clase Equipo
class Equipo {
private:
    vector<Jugador*> jugadores;
    char nombreEquipo; // 'X' o 'O'
    int bandera_x; // Coordenada X de la bandera
    vector<int> bandera_y; // Coordenadas Y posibles de la bandera

public: // Constructor
    Equipo(char nombreEquipo) : nombreEquipo(nombreEquipo) {}
    void agregarJugador(Jugador* jugador) { jugadores.push_back(jugador); }
    void removerJugador(int id);
    bool verificarVictoria(int ancho);
    bool verificarRetirada();
    // Getters
    vector<Jugador*>& getJugadores() { return jugadores; }
    int getBanderaX() { return bandera_x; }
    vector<int>& getBanderaY() { return bandera_y; }
    void setBandera(int x, const vector<int>& y_coords) { bandera_x = x; bandera_y = y_coords; }
    char getNombreEquipo() { return nombreEquipo; }
};

// Clase Tablero
class Tablero {
private:
    int ancho;
    int alto;
    vector<vector<vector<Jugador*>>> grid;

public:
    Tablero(int ancho, int alto) : ancho(ancho), alto(alto) {
        grid = vector<vector<vector<Jugador*>>>(ancho, vector<vector<Jugador*>>(alto));
    }

    bool validarMovimientoParcial(int x, int y, Jugador& jugador);
    void moverJugador(Jugador& jugador, int nuevo_x, int nuevo_y);
    bool agregarJugador(Jugador& jugador);
    void removerJugador(Jugador& jugador);
    vector<Jugador*> getJugadoresEnPosicion(int x, int y);
    void mostrarTablero();
    int getAncho() { return ancho; }
    int getAlto() { return alto; }
};

// Clase Juego
class Juego {
private:
    Tablero tablero;
    Equipo equipo1;
    Equipo equipo2;
    int turnoActual; // 1 o 2
    int numeroTurnos;
    Jugador* jugadorActual;

public:
    Juego();
    void iniciar();
    void ejecutarTurno();
    void verificarFinDeJuego();
    Tablero& getTablero() { return tablero; }
    Equipo& getEquipoEnTurno();
    Jugador* getJugadorActual() { return jugadorActual; }
    void setJugadorActual(Jugador* jugador) { jugadorActual = jugador; }
    Equipo& getOtroEquipo();
};

// Clase Comandos
class Comandos {
public:
    static bool interpretar(string input, Juego& juego);
};

// Implementación de métodos de Jugador
bool Jugador::mover(vector<string> direcciones, Tablero& tablero) {
    int maxMovimiento = (tipo == "rápido") ? 2 : 1; // Corregido: "rápido" con acento
    if (direcciones.size() > maxMovimiento) {
        cout << "Movimiento excede el máximo permitido para este jugador." << endl;
        return false;
    }

    int x_actual = x;
    int y_actual = y;

    for (const string& direccion : direcciones) {
        if (direccion == "u") {
            y_actual -= 1;
        } else if (direccion == "d") {
            y_actual += 1;
        } else if (direccion == "l") {
            x_actual -= 1;
        } else if (direccion == "r") {
            x_actual += 1;
        } else {
            cout << "Dirección inválida: " << direccion << endl;
            return false;
        }

        // Validar movimiento parcial
        if (!tablero.validarMovimientoParcial(x_actual, y_actual, *this)) {
            cout << "Movimiento no permitido. Camino bloqueado, fuera de límites, o casilla llena." << endl;
            return false;
        }
    }

    // Movimiento válido, actualizar posición
    tablero.moverJugador(*this, x_actual, y_actual);
    cout << "Movimiento exitoso." << endl;
    return true;
}

bool Jugador::atacar(int objetivo_x, int objetivo_y, Tablero& tablero) {
    int distancia = abs(objetivo_x - x) + abs(objetivo_y - y);

    // Determinar alcance y probabilidad de acierto según experiencia
    double probabilidadAcierto = 0.0;
    int alcanceMaximo = 0;

    if (experiencia == "novato") {
        alcanceMaximo = 1;
        if (distancia > alcanceMaximo) {
            cout << "Objetivo fuera de alcance para un jugador novato." << endl;
            return false;
        }
        probabilidadAcierto = 0.85; // Probabilidad total de acierto
    } else { // Experto
        if (distancia == 1) {
            probabilidadAcierto = 0.75;
        } else if (distancia == 2) {
            probabilidadAcierto = 0.25;
        } else {
            cout << "Objetivo fuera de alcance para un jugador experto." << endl;
            return false;
        }
    }

    // Intentar acertar al objetivo
    double dadoAcierto = (double)rand() / RAND_MAX;
    if (dadoAcierto <= probabilidadAcierto) {
        auto objetivos = tablero.getJugadoresEnPosicion(objetivo_x, objetivo_y);
        bool impactoRealizado = false;
        for (auto& objetivo : objetivos) {
            if (objetivo->getEquipo() != equipo) {
                // Determinar parte del cuerpo impactada
                double dadoParte = (double)rand() / RAND_MAX;
                string parteImpactada;

                if (experiencia == "novato") {
                    // Probabilidades normalizadas
                    if (dadoParte <= 0.1176) {
                        parteImpactada = "torso";
                    } else if (dadoParte <= 0.1176 + 0.5882) {
                        parteImpactada = "extremidad";
                    } else {
                        parteImpactada = "cabeza";
                    }
                } else { // Experto
                    // Probabilidades normalizadas
                    if (dadoParte <= 0.4) {
                        parteImpactada = "torso";
                    } else if (dadoParte <= 0.4 + 0.5667) {
                        parteImpactada = "extremidad";
                    } else {
                        parteImpactada = "cabeza";
                    }
                }

                objetivo->recibirImpacto(parteImpactada, tablero, *this);
                cout << "Jugador " << equipo << id << " ha impactado al jugador "
                     << objetivo->getEquipo() << objetivo->getId() << " en " << parteImpactada << "." << endl;
                impactoRealizado = true;
            }
        }
        if (!impactoRealizado) {
            cout << "No hay enemigos en la posición objetivo." << endl;
            return false;
        }
        return true;
    } else {
        cout << "El ataque ha fallado." << endl;
        return false;
    }
}

// Modificación: Actualizamos 'recibirImpacto' para implementar las nuevas reglas
void Jugador::recibirImpacto(string parte, Tablero& tablero, Jugador& atacante) {
    if (parte == "cabeza") {
        // El atacante es eliminado inmediatamente
        atacante.eliminado = true;
        atacante.estado = "eliminado";
        cout << "Jugador " << atacante.getEquipo() << atacante.getId()
             << " ha sido eliminado por impactar en la cabeza del oponente." << endl;
        tablero.removerJugador(atacante);
    } else if (parte == "torso") {
        eliminado = true;
        estado = "eliminado";
        cout << "Jugador " << equipo << id << " ha sido eliminado por impacto en el torso." << endl;
        tablero.removerJugador(*this);
    } else if (parte == "extremidad") {
        impactosExtremidades++;
        estado = "herido en extremidad";
        cout << "Jugador " << equipo << id << " ha sido herido en una extremidad." << endl;
        if (impactosExtremidades >= 3) {
            eliminado = true;
            estado = "eliminado";
            cout << "Jugador " << equipo << id << " ha sido eliminado por acumular tres impactos en extremidades." << endl;
            tablero.removerJugador(*this);
        }
    } else {
        // En caso de una parte no contemplada, no hacer nada
    }
}

void Jugador::mostrarEstado() {
    cout << "Jugador " << equipo << id << " (" << tipo << ", " << experiencia
         << ") en (" << x << "," << y << ") estado: " << estado << endl;
}

// Implementación de métodos de Equipo
void Equipo::removerJugador(int id) {
    jugadores.erase(remove_if(jugadores.begin(), jugadores.end(),
                              [id](Jugador* j) { return j->getId() == id; }),
                    jugadores.end());
}

bool Equipo::verificarVictoria(int ancho) {
    for (auto jugador : jugadores) {
        if (jugador->estaEliminado()) continue;
        if (jugador->getEquipo() == 'X' && jugador->getX() == ancho - 1) {
            return true;
        } else if (jugador->getEquipo() == 'O' && jugador->getX() == 0) {
            return true;
        }
    }
    return false;
}

bool Equipo::verificarRetirada() {
    return all_of(jugadores.begin(), jugadores.end(),
                  [](Jugador* j) { return j->estaEliminado(); });
}

// Implementación de métodos de Tablero
bool Tablero::validarMovimientoParcial(int x, int y, Jugador& jugador) {
    if (x < 0 || x >= ancho || y < 0 || y >= alto) {
        return false;
    }

    auto& cell = grid[x][y];

    // Contar jugadores del mismo equipo en la casilla
    int countSameTeam = 0;
    for (auto& otroJugador : cell) {
        if (otroJugador->getEquipo() != jugador.getEquipo()) {
            return false; // No puede moverse a una casilla con enemigos
        } else {
            if (!otroJugador->estaEliminado()) {
                countSameTeam++;
            }
        }
    }

    if (countSameTeam >= 4) {
        return false; // Casilla llena para este equipo
    }

    return true;
}

void Tablero::moverJugador(Jugador& jugador, int nuevo_x, int nuevo_y) {
    removerJugador(jugador);
    jugador.setPosicion(nuevo_x, nuevo_y);
    agregarJugador(jugador);
}

bool Tablero::agregarJugador(Jugador& jugador) {
    int x = jugador.getX();
    int y = jugador.getY();
    if (x < 0 || x >= ancho || y < 0 || y >= alto) {
        cout << "Error: Posición fuera de los límites del tablero." << endl;
        return false;
    }

    auto& cell = grid[x][y];

    // Contar jugadores del mismo equipo en la casilla
    int countSameTeam = 0;
    for (auto& otroJugador : cell) {
        if (otroJugador->getEquipo() == jugador.getEquipo() && !otroJugador->estaEliminado()) {
            countSameTeam++;
        }
    }

    if (countSameTeam >= 4) {
        cout << "No se puede agregar al jugador " << jugador.getEquipo() << jugador.getId()
             << " en (" << x << "," << y << "). Casilla llena para este equipo." << endl;
        return false;
    }

    cell.push_back(&jugador);
    return true;
}

void Tablero::removerJugador(Jugador& jugador) {
    int x = jugador.getX();
    int y = jugador.getY();
    if (x < 0 || x >= ancho || y < 0 || y >= alto) {
        return;
    }
    auto& cell = grid[x][y];
    cell.erase(remove(cell.begin(), cell.end(), &jugador), cell.end());
}

vector<Jugador*> Tablero::getJugadoresEnPosicion(int x, int y) {
    if (x < 0 || x >= ancho || y < 0 || y >= alto) {
        return {};
    }
    return grid[x][y];
}

// Modificación en 'mostrarTablero' para agregar '*'
void Tablero::mostrarTablero() {
    cout << "Estado del tablero:" << endl;
    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            auto& cell = grid[x][y];
            if (!cell.empty()) {
                string contenido = "";
                for (auto jugador : cell) {
                    if (!jugador->estaEliminado()) {
                        contenido += jugador->getEquipo();
                        contenido += to_string(jugador->getId());
                        // Agregar '*' si tiene impactos en extremidades
                        if (jugador->getImpactosExtremidades() > 0) {
                            contenido += "*";
                        }
                        contenido += ",";
                    }
                }
                if (!contenido.empty()) {
                    contenido.pop_back(); // Remover la última coma
                    cout << "[" << contenido << "]";
                } else {
                    cout << " . ";
                }
            } else {
                cout << " . ";
            }
            cout << " ";
        }
        cout << endl;
    }
}

// Declaración de la función para generar tipo y experiencia
void generarTipoYExperiencia(string& tipo, string& experiencia);

// Implementación de métodos de Juego
Juego::Juego()
    : tablero(10, 10), equipo1('X'), equipo2('O'), numeroTurnos(0), jugadorActual(nullptr) {
    srand(time(0));

    // Generar dimensiones aleatorias para el tablero
    int min_ancho = 4, max_ancho = 12;
    int min_alto = 4, max_alto = 10;
    int ancho = rand() % (max_ancho - min_ancho + 1) + min_ancho;
    int alto = rand() % (max_alto - min_alto + 1) + min_alto;

    // Actualizar el tablero con las dimensiones aleatorias
    tablero = Tablero(ancho, alto);

    // Determinar aleatoriamente quién inicia el juego
    turnoActual = rand() % 2 + 1; // Genera 1 o 2
}

void Juego::iniciar() {
    // Generar número aleatorio de jugadores
    int min_jugadores = 4, max_jugadores = 10;
    int num_jugadores = rand() % (max_jugadores - min_jugadores + 1) + min_jugadores;

    int ancho = tablero.getAncho();
    int alto = tablero.getAlto();

    // Ubicar banderas
    vector<int> bandera_y_X; // Coordenadas Y para la bandera del equipo X
    vector<int> bandera_y_O; // Coordenadas Y para la bandera del equipo O

    for (int y = 0; y < alto; ++y) {
        bandera_y_X.push_back(y);
        bandera_y_O.push_back(y);
    }

    equipo1.setBandera(ancho - 1, bandera_y_X); // Bandera del equipo 'X' en el extremo opuesto
    equipo2.setBandera(0, bandera_y_O);         // Bandera del equipo 'O' en el extremo opuesto

    // Crear jugadores y ubicarlos en los extremos
    for (int i = 0; i < num_jugadores; ++i) {
        // Generar tipo y experiencia aleatoria para el jugador del equipo 'X'
        string tipoX, experienciaX;
        generarTipoYExperiencia(tipoX, experienciaX);

        int pos_y_X = i % alto;
        Jugador* jugador1 = new Jugador(tipoX, experienciaX, 'X', i + 1, 0, pos_y_X);
        equipo1.agregarJugador(jugador1);
        if (!tablero.agregarJugador(*jugador1)) {
            cout << "Error al agregar jugador " << jugador1->getId() << " al tablero." << endl;
            exit(1);
        }

        // Generar tipo y experiencia aleatoria para el jugador del equipo 'O'
        string tipoO, experienciaO;
        generarTipoYExperiencia(tipoO, experienciaO);

        int pos_y_O = i % alto;
        Jugador* jugador2 = new Jugador(tipoO, experienciaO, 'O', i + 1, ancho - 1, pos_y_O);
        equipo2.agregarJugador(jugador2);
        if (!tablero.agregarJugador(*jugador2)) {
            cout << "Error al agregar jugador " << jugador2->getId() << " al tablero." << endl;
            exit(1);
        }
    }

    // Mostrar información inicial
    cout << "Dimensiones del tablero: " << ancho << "x" << alto << endl;
    cout << "Número de jugadores por equipo: " << num_jugadores << endl;
    cout << "El equipo que inicia el juego es: " << ((turnoActual == 1) ? 'X' : 'O') << endl;

    // Bucle principal del juego
    while (true) {
        ejecutarTurno();
        verificarFinDeJuego();
        numeroTurnos++;
    }
}

void Juego::ejecutarTurno() {
    Equipo& equipoEnTurno = getEquipoEnTurno();
    cout << "Turno del equipo " << equipoEnTurno.getNombreEquipo() << endl;

    tablero.mostrarTablero();

    // Mostrar los jugadores disponibles
    cout << "Jugadores disponibles:" << endl;
    for (auto jugador : equipoEnTurno.getJugadores()) {
        if (!jugador->estaEliminado()) {
            jugador->mostrarEstado();
        }
    }

    // Pedir al usuario que seleccione un jugador
    string linea;
    int idSeleccionado;
    cout << "Ingrese el ID del jugador que desea usar: ";
    getline(cin, linea);
    stringstream ss(linea);
    if (!(ss >> idSeleccionado)) {
        cout << "Entrada inválida. Pierdes el turno." << endl;
    } else {
        // Buscar el jugador seleccionado
        Jugador* jugadorSeleccionado = nullptr;
        for (auto jugador : equipoEnTurno.getJugadores()) {
            if (!jugador->estaEliminado() && jugador->getId() == idSeleccionado) {
                jugadorSeleccionado = jugador;
                break;
            }
        }

        if (!jugadorSeleccionado) {
            cout << "Jugador no encontrado o eliminado. Pierdes el turno." << endl;
        } else {
            jugadorActual = jugadorSeleccionado;

            // Pedir comando
            cout << "Ingrese comando para el jugador " << jugadorActual->getId() << ": ";
            getline(cin, linea);

            if (linea == "salir") {
                cout << "Has salido del juego." << endl;
                exit(0);
            }

            if (!Comandos::interpretar(linea, *this)) {
                cout << "Comando inválido o acción fallida. Pierdes el turno." << endl;
            }
        }
    }

    // Cambiar de turno
    turnoActual = (turnoActual == 1) ? 2 : 1;
}

void Juego::verificarFinDeJuego() {
    int ancho = tablero.getAncho();

    if (equipo1.verificarRetirada()) {
        cout << "El equipo O gana en " << numeroTurnos << " turnos." << endl;
        exit(0);
    } else if (equipo2.verificarRetirada()) {
        cout << "El equipo X gana en " << numeroTurnos << " turnos." << endl;
        exit(0);
    }

    if (equipo1.verificarVictoria(ancho)) {
        cout << "El equipo X captura la bandera y gana en " << numeroTurnos << " turnos." << endl;
        exit(0);
    } else if (equipo2.verificarVictoria(ancho)) {
        cout << "El equipo O captura la bandera y gana en " << numeroTurnos << " turnos." << endl;
        exit(0);
    }
}

Equipo& Juego::getEquipoEnTurno() {
    return (turnoActual == 1) ? equipo1 : equipo2;
}

// Implementación de métodos de Comandos
bool Comandos::interpretar(string input, Juego& juego) {
    istringstream iss(input);
    string accion;
    iss >> accion;
    Jugador* jugador = juego.getJugadorActual();
    if (!jugador) return false;

    if (accion == "mover") {
        vector<string> direcciones;
        string direccion;
        while (iss >> direccion) {
            direcciones.push_back(direccion);
        }
        if (direcciones.empty()) {
            cout << "Debe proporcionar al menos una dirección para moverse." << endl;
            return false;
        }
        return jugador->mover(direcciones, juego.getTablero());
    } else if (accion == "atacar") {
        int x, y;
        iss >> x >> y;
        if (iss.fail()) {
            cout << "Comando 'atacar' requiere dos coordenadas numéricas." << endl;
            return false;
        }
        return jugador->atacar(x, y, juego.getTablero());
    } else {
        cout << "Comando inválido." << endl;
        return false;
    }
}

// Nueva función para generar tipo y experiencia basados en probabilidades
void generarTipoYExperiencia(string& tipo, string& experiencia) {
    // Generar un número aleatorio entre 0 y 1
    double random = (double)rand() / RAND_MAX;

    // Probabilidades normalizadas
    double prob_ER = 0.0909; // 9.09%
    double prob_EL = 0.1515; // 15.15%
    double prob_NR = 0.3030; // 30.30%
    double prob_NL = 0.4545; // 45.45%

    if (random < prob_ER) {
        experiencia = "experto";
        tipo = "rápido";
    } else if (random < prob_ER + prob_EL) {
        experiencia = "experto";
        tipo = "lento";
    } else if (random < prob_ER + prob_EL + prob_NR) {
        experiencia = "novato";
        tipo = "rápido";
    } else {
        experiencia = "novato";
        tipo = "lento";
    }
}

// Función main
int main() {
    srand(time(0)); // Inicializar la semilla
    Juego juego;
    juego.iniciar();
    return 0;
}
