#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

// Configurações Wi-Fi
#define WIFI_SSID "LINE"
#define WIFI_PASSWORD "SANTANA0411"

// Pinos
#define BOTAO_A 5
#define JOYSTICK_X 27
#define JOYSTICK_Y 26

// Variáveis globais
char status_B[50] = "Botao A: nao pressionado";
char joystick_dir[50] = "Centro";



// Função para detectar direção do joystick
const char* get_joystick_dir(int x, int y) {
    if (x < 1500 && y > 2500) return "Noroeste";
    if (x > 2500 && y > 2500) return "Nordeste";
    if (x < 1500 && y < 1500) return "Sudoeste";
    if (x > 2500 && y < 1500) return "Sudeste";
    if (x < 1500) return "Oeste";
    if (x > 2500) return "Leste";
    if (y > 2500) return "Norte";
    if (y < 1500) return "Sul";
    return "Centro";
}

// Atualiza estado dos sensores
void ler_sensor() {
    static bool b1_last = false;
    bool b1 = !gpio_get(BOTAO_A);

    if (b1 != b1_last) {
        snprintf(status_B, sizeof(status_B), "Botao A: %s", b1 ? "pressionado" : "nao pressionado");
        b1_last = b1;
    }

    adc_select_input(1); int x = adc_read();
    adc_select_input(0); int y = adc_read();
    // Linha modificada abaixo (agora mostra X e Y):
    snprintf(joystick_dir, sizeof(joystick_dir), "Joystick: %s (X=%d, Y=%d)", get_joystick_dir(x, y), x, y);
}

// Resposta ao navegador
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    ler_sensor();

    char html[1024];  // Aumente o tamanho para caber o novo estilo
snprintf(html, sizeof(html),
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<!DOCTYPE html><html>"
    "<head><title>Monitor BitdogLab</title>"
    "<meta http-equiv='refresh' content='1'>"
    "<style>"
    "body { font-family: Arial; text-align: center; margin-top: 50px; background: #f0f8ff; }"
    "h1 { color: #2c3e50; }"
    ".sensor { background: white; border: 2px solid #3498db;"
    "border-radius: 10px; padding: 15px; margin: 10px auto;"
    "width: 300px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }"
    "</style></head>"
    "<body>"
    "<h1>Monitor BitdogLab</h1>"
    "<div class='sensor'>%s</div>"
    "<div class='sensor'>%s</div>"
    "</body></html>",
    status_B, 
    joystick_dir);

    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    free(request);
    pbuf_free(p);
    return ERR_OK;
}

// Aceita conexão
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

int main() {
    stdio_init_all();
    gpio_init(BOTAO_A); gpio_set_dir(BOTAO_A, GPIO_IN); gpio_pull_up(BOTAO_A);
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    if (cyw43_arch_init()) {
        printf("Falha ao iniciar Wi-Fi\n");
        return -1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Falha ao conectar\n");
        return -1;
    }

    printf("Conectado. IP: %s\n", ipaddr_ntoa(&netif_default->ip_addr));

    // Servidor TCP
    struct tcp_pcb *server = tcp_new();
    if (!server || tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Erro ao criar servidor\n");
        return -1;
    }

    server = tcp_listen(server);
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");

    // Loop principal
    while (true) {
        ler_sensor();
        cyw43_arch_poll();
        sleep_ms(1000);
    }

    cyw43_arch_deinit();
    return 0;
}