#pragma once
#include <random>
#include <string>
#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif

namespace Engine {
enum class Currency { USD };
class Utils {
public:
  std::string getRandomJoke() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, shittyJokes.size() - 1);
    size_t idx = dist(rng);
    return shittyJokes[idx];
  }
  static std::string formatCoin(int value, Currency curr) {
    std::string inteiroStr = std::to_string(value);
    std::string result;
    int count = 0;
    for (int i = static_cast<int>(inteiroStr.size()) - 1; i >= 0; --i) {
      result = inteiroStr[i] + result;
      count++;

      if (count == 3 && i > 1) {
        result = "." + result;
        count = 0;
      }
    }

    const char *symbols[] = {"$"};
    return std::string(symbols[static_cast<int>(curr)]) + result + ",00";
  }

private:
  std::vector<std::string> shittyJokes = {
      "Mais perdido que GPS sem sinal no meio do mato.",
      "Mais liso que sabonete fugindo da polícia.",
      "Mais devagar que download de 1GB na internet da roça.",
      "Mais feio que bater na mãe por causa de coxinha.",
      "Mais apertado que sardinha em lata de promoção.",
      "Mais quente que tampa de fogão em dia de almoço de domingo.",
      "Mais quebrado que cadeira de plástico em churrasco.",
      "Mais enrolado que fone de ouvido no bolso.",
      "Mais inútil que limpa-para-brisa em submarino.",
      "Mais nervoso que goleiro em cobrança de pênalti.",
      "Mais perdido que cebola em salada de fruta.",
      "Mais falso que nota de três reais.",
      "Mais cansado que burro de mudança.",
      "Mais rápido que notícia ruim em grupo de família.",
      "Mais duro que pão esquecido atrás da geladeira.",
      "Mais estranho que pastel de vento recheado de ar.",
      "Mais confuso que cego em tiroteio de filme de ação.",
      "Mais azarado que jogador que toma disconnect na final.",
      "Mais lotado que ônibus em véspera de feriado.",
      "Mais seco que humor de programador em produção.",
      "Mais inútil que botão de elevador já apertado.",
      "Mais bravo que galo acordando às quatro da manhã.",
      "Mais apertado que calça comprada pela internet.",
      "Mais antigo que tutorial em Flash.",
      "Mais torto que antena improvisada com garfo.",
      "Mais sumido que meia depois da lavagem.",
      "Mais suspeito que Wi-Fi sem senha.",
      "Mais perdido que pinguim no deserto.",
      "Mais aleatório que bug corrigido sozinho.",
      "Mais rápido que promoção acabando no carrinho.",
      "Mais enrolado que código legado sem comentário.",
      "Mais forte que café de pedreiro.",
      "Mais frio que coração de gerente cobrando meta.",
      "Mais teimoso que atualização do Windows.",
      "Mais barulhento que moto sem escapamento.",
      "Mais imprevisível que estagiário com acesso ao banco de dados.",
      "Mais apertado que vaga de estacionamento em shopping.",
      "Mais feliz que cachorro vendo o dono chegar.",
      "Mais assustado que programador vendo 'DELETE FROM' sem WHERE.",
      "Mais liso que chão molhado com detergente."};
};
} // namespace Engine
