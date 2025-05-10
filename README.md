# Cenário de Aplicação: Painel de Comunicação Alternativa em Casas de Repouso

Idosos com dificuldade de fala usam o joystick para navegar por símbolos/palavras e se comunicar com cuidadores. A Raspberry Pi Pico W (BitdogLab) lê essas interações e disponibiliza feedback em um navegador local, permitindo mensagens claras e autônomas.

## Descrição do Projeto

A Pico W atua como servidor HTTP embarcado.  
A cada segundo, o estado do botão (pressionado/não pressionado) e a posição X/Y do joystick são coletados.  
Essas informações são apresentadas em tempo real em uma página web estilizada, acessível pelo IP exibido no serial monitor.

## Como Funciona

Imagine um painel de comunicação interativa fixado próximo à poltrona dos residentes. Idosos com dificuldades de fala utilizam o joystick para navegar entre símbolos (como "água", "ajuda", "alimentação", "banheiro") e pressionam um botão físico para confirmar sua escolha. O sistema funciona da seguinte forma:

- **Joystick**: Mapeia direções em oito zonas (Norte, Sul, Leste, Oeste, Nordeste, Noroeste, Sudeste, Sudoeste) e corresponde a opções na tela.
- **Botão**: Confirma a escolha feita.
- **Interface Web**: Exibe em tempo real a seleção feita e permite que cuidadores acompanhem sem precisar de telas específicas.
- **Auto-refresh**: A página HTML recarrega automaticamente a cada segundo.

## Comunicação com Servidor

A Pico W executa um servidor HTTP local.  
Não há dependência de servidores externos.  
Usuários acessam pelo IP da placa para visualizar dados atualizados.

## Benefícios do Sistema

- **Inclusão Digital**: Comunicação simples e tátil.
- **Feedback em Tempo Real**: Atualizações sem recarga manual.
- **Autonomia**: Processamento e hospedagem na própria placa.
- **Economia**: Hardware de baixo custo.
