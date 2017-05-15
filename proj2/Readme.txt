Elementos do grupo:
  David Falcão
  João Mendes
  Nuno Pereira

Race conditions:
  Utilizamos um mutex para evitar que 2 threads tentem aceder em simultâneo à mesma variável


  SAUNA:
  >Invocação: sauna <capacidade máxima>
  >Registo: /tmp/bal.<pid do processo>

  GERADOR:
  >Invocação: gerador <n.pedidos> <duração máxima por pedido>
  >Registo: /tmp/ger.<pid do processo>
