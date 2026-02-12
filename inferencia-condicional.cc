/**
 * Universidad de La Laguna
 * Escuela Superior de Ingeniería y Tecnología
 * Grado en Ingeniería Informática
 * Inteligencia Artificial Avanzada
 * 
 * @author Cristhian Adoney Cruz Delgado (alu0101648293@ull.edu.es)
 * @author Michelangelo da Cruz Bochicchio (alu0101651217@ull.edu.es)
 * @since Feb 12 2026
 * 
 */

#include <bitset>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <vector>

/* 2. */

// Convierte una máscara a índice del array de propabilidades 
int indiceP(const std::string& mascara) {
  int indice = 0;
  int exponente = 0;
  for (int i = mascara.length() - 1; i >= 0; --i) {
    if (mascara[i] == '1') {
      indice |= (1 << exponente);
    }
    ++exponente;
  }
  return indice;
}

/* 1.1. y 1.2. */

bool cargarDistribucionCSV(std::vector<double>& p, int& N) {
  std::ifstream input_file("./input.csv");
  if (!input_file.is_open()) {
    std::cerr << "Error: No se pudo leer el archivo input.csv\n";
    return false;
  }

  std::string line;
  if (!std::getline(input_file, line)) {
    std::cerr << "Error: Archivo CSV vacío.\n";
    return false;
  }

  std::string mascara;
  std::string probabilidad;

  std::stringstream string_stream(line);

  std::getline(string_stream, mascara, ',');
  std::getline(string_stream, probabilidad);

  N = mascara.length();
  p.resize(1 << N, 0.0);
  if (N <= 0 || N >= 33) {
    std::cerr << "Error: Número de variables no válido.\n";
    return false;
  }

  int k = indiceP(mascara);
  p[k] = std::stod(probabilidad);

  // Procesar resto de líneas
  while (std::getline(input_file, line)) {
    std::stringstream string_stream(line);

    std::getline(string_stream, mascara, ',');
    std::getline(string_stream, probabilidad);

    int k = indiceP(mascara);
    p[k] = std::stod(probabilidad);
  }

  return true;
}

bool generarDistribucionAleatoria(std::vector<double>& p, int& N) {
  std::cout << "Elija el número de variables a generar: ";
  std::cin >> N;
  if (N <= 0 || N >= 33) {
    std::cerr << "Error: Número de variables no válido.\n";
    return false;
  }

  p.resize(1 << N, 0.0);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // Generar y después normalizar
  double suma = 0.0;
  for (int i = 0; i < p.size(); ++i) {
    double numero_aleatorio = dist(gen);
    p[i] = numero_aleatorio;
    suma += numero_aleatorio;
  }
  for (int i = 0; i < p.size(); ++i) {
    p[i] /= suma;
  }

  return true;
}

/* 3. */

bool seleccionarVariablesCondicionales(int& maskC, int& valC, int N) {
  std::cout << "\n¿Cuántas variables condicionales quiere seleccionar?\n";
  int variables_condicionales;    
  std::cin >> variables_condicionales;
  std::cout << "\nA continuación se le pedirá que introduzca " << variables_condicionales 
            << " variable/s una a una, indique el índice de la variable condicionada (1 a " 
            << N << ") y su valor (0 o 1) separado por un espacio.\n\n";
    
  for (int i = 0; i < variables_condicionales; ++i) {
    std::cout << "Variable valor: ";
    int variable, valor;
    std::cin >> variable >> valor;
    if (variable > N || (valor != 0 && valor != 1)) {
      std::cerr << "Error al seleccionar variable condicionada, debe estar entre 1-" << N << " y con valor 0 o 1." << std::endl;
      return false;
    }
    --variable;
    
    // Activamos el bit correspondiente a la posición de la variable
    maskC |= (1 << variable);
    if (valor == 1) valC |= (1 << variable);
  }
  return true;
}

bool seleccionarVariablesInteres(int& maskI, int N) {
  std::cout << "\n¿Cuántas variables de interés quiere seleccionar?\n";
  int variables_interes;    
  std::cin >> variables_interes;
  std::cout << "\nA continuación se le pedirá que introduzca " << variables_interes 
            << " variable/s una a una, indique el índice de la variable de interés (1 a " 
            << N << ").\n\n";
    
  for (int i = 0; i < variables_interes; ++i) {
    int variable;
    std::cout << "Variable: ";
    std::cin >> variable;
    if (variable > N) {
      std::cerr << "Error al seleccionar variable condicionada, debe estar entre 1-" << N << std::endl;
      return false;
    }
    --variable;

    // Activamos el bit correspondiente a la posición de la variable
    maskI |= (1 << variable);
  }
  return true;
}

/* 5. */

std::vector<double> calcularProbabilidadCondicional(std::vector<double>& p, int& N, int& maskC, int& valC, int& maskI) {  
  // Calcular denominador P(maskC = valC)
  double suma = 0.0;
  for (int i = 0; i < p.size(); ++i) {
    if ((i & maskC) == (valC & maskC)) {
      suma += p[i];
    }
  }
  
  // Si la probabilidad de la condición es 0, no se puede calcular
  if (suma == 0.0) {
    std::cerr << "Error: P(maskC = valC) = 0, no se puede condicionar.\n";
    return std::vector<double>();
  }

  // Calcular tamaño de numerador P(maskI)
  int k = 0;
  for (int i = 0; i < N; ++i) {
    if (maskI & (1 << i)) k++;
  }

  std::vector<double> p_condicional(1 << k, 0.0);

  // Calcular numerador P(maskI)
  for (int i = 0; i < p.size(); ++i) {
    if ((i & maskC) == (valC & maskC)) {
      int j = 0;
      int pos = 0;
      for (int b = 0; b < N; ++b) {
        if (maskI & (1 << b)) {
          if (i & (1 << b)) j |= (1 << pos);
          pos++;
        }
      }

      p_condicional[j] += p[i];
    }
  }

  // Normalizar
  for (int j = 0; j < p_condicional.size(); ++j) {
    p_condicional[j] /= suma;
  }

  return p_condicional;
}

int main() {
    
  /* 1. Carga de la distribución conjunta */
  /* 2. Carga de las probabilidades en un array de probabilidades */

  std::cout << "\nElija el método de carga de la distribución conjunta (leer CSV|generar aleatoriamente): ";
  std::string tipo_carga;
  std::getline(std::cin, tipo_carga);

  int N = 0;
  std::vector<double> p;

  if (tipo_carga == "1" || tipo_carga == "leer CSV") {
    if (cargarDistribucionCSV(p, N)) {
      std::cout << "CSV cargado.\n";
    } else {
      std::cerr << "Error: bug en carga.\n";
      return 1;
    }
  } else if (tipo_carga == "2" || tipo_carga == "generar aleatoriamente") {
    if (generarDistribucionAleatoria(p, N)) {
      std::cout << "Probabilidad aleatoria cargada.\n";
    } else {
      std::cerr << "Error: bug en carga.\n";
      return 1;
    }
  } else {
    std::cerr << "Error: Opción no válida.\n";
    return 1;
  }

  /* 3. Selección de variables */
  /* 4. Uso de máscaras en el cálculo de la probabilidad condicional */

  // P(maskI | maskC = valC)

  int maskC = 0;
  int valC = 0;
  int maskI = 0;

  if (!(seleccionarVariablesCondicionales(maskC, valC, N))) return 1;
  if (!(seleccionarVariablesInteres(maskI, N))) return 1;

  /* 5. Función principal del cálculo */
  /* 7. Estudio del tiempo de ejecución */

  std::chrono::high_resolution_clock::time_point beg = std::chrono::high_resolution_clock::now();
  std::vector<double> p_condicionada = calcularProbabilidadCondicional(p, N, maskC, valC, maskI);
  std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
  std::chrono::microseconds µs = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
  std::cout << "\nEl tiempo de ejecución han sido: " << µs.count() << " µs\n";

  /* 6. Salida del programa */

  std::cout << "\nEste tiempo ha tomado sobre la distribución de probabilidad:\n";

  for (int i = 0; i < p.size(); ++i) {
    std::bitset<32> all_bits(i);
    std::string real_bits = all_bits.to_string().substr(32 - N, N);
    std::cout << real_bits << ' ' << p[i] << '\n';
  }

  std::bitset<32> all_bitsI(maskI), all_bitsC(maskC), all_bitsValC(valC);
  std::string real_bitsI = all_bitsI.to_string().substr(32 - N, N);
  std::string real_bitsC = all_bitsC.to_string().substr(32 - N, N);
  std::string real_bitsValC = all_bitsValC.to_string().substr(32 - N, N);
  std::cout << "\nCalcular P(" << real_bitsI << " | " << real_bitsC << " = " << real_bitsValC << "):\n";

  for (int i = 0; i < p_condicionada.size(); ++i) {
    std::cout << p_condicionada[i] << '\n';
  }

  std::cout << '\n';

  return 0;
}