import pandas as pd
import matplotlib.pyplot as plt

CAMINHO_CSV = "TestePosicionamento.csv"
CAMINHO_PLOT = "Graficos/"

def plot_step_response(csv_file):
    # Carrega os dados
    df = pd.read_csv(csv_file)
    
    # Remove eventuais linhas com dados faltantes (NaN)
    df = df.dropna()

    # Converte o tempo de milissegundos para segundos
    df['Tempo(s)'] = df['Tempo(ms)'] / 1000.0

    df_zoom1 = df[(df['Tempo(s)'] >= 0) & (df['Tempo(s)'] <= 4)]
    df_zoom2 = df[(df['Tempo(s)'] >= 4) & (df['Tempo(s)'] <= 7)]

    # --- Plot 1: Step Response (Visão Geral) ---
    plt.figure(figsize=(10, 6))
    plt.plot(df['Tempo(s)'], df['Alvo(deg)'], '--', label='Referência (Alvo)', color='tab:blue')
    plt.plot(df['Tempo(s)'], df['Real(deg)'], '-', label='Resposta Real', color='tab:orange')
    plt.xlabel('Tempo (s)')
    plt.ylabel('Resposta (graus)')
    plt.title('Resposta ao Degrau - Visão Geral')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    plt.savefig(CAMINHO_PLOT + 'step_response_geral.png', dpi=300)

    # --- Plot 2: Magnified Step Response (Zoom no Transiente) de 0 para 360---
    plt.figure(figsize=(10, 6))
    plt.plot(df_zoom1['Tempo(s)'], df_zoom1['Alvo(deg)'], '--', label='Referência', color='tab:blue')
    plt.plot(df_zoom1['Tempo(s)'], df_zoom1['Real(deg)'], '-', label='Resposta Real', color='tab:orange')
    plt.xlabel('Tempo (s)')
    plt.ylabel('Resposta (graus)')
    plt.title('Resposta ao Degrau - Zoom no Transiente (0 a 360 graus)')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    plt.savefig(CAMINHO_PLOT + 'magnified_step_response_1.png', dpi=300)

    # --- Plot 3: Magnified Step Response (Zoom no Transiente) de 360 pra 0---
    plt.figure(figsize=(10, 6))
    plt.plot(df_zoom2['Tempo(s)'], df_zoom2['Alvo(deg)'], '--', label='Referência', color='tab:blue')
    plt.plot(df_zoom2['Tempo(s)'], df_zoom2['Real(deg)'], '-', label='Resposta Real', color='tab:orange')
    plt.xlabel('Tempo (s)')
    plt.ylabel('Resposta (graus)')
    plt.title('Resposta ao Degrau - Zoom no Transiente (360 a 0 graus)')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    plt.savefig(CAMINHO_PLOT + 'magnified_step_response_2.png', dpi=300)

    # --- Plot 4: Erro de Posicionamento ---
    plt.figure(figsize=(10, 6))
    plt.plot(df['Tempo(s)'], df['Erro(deg)'], '-', color='tab:blue')
    plt.xlabel('Tempo (s)')
    plt.ylabel('Erro (graus)')
    plt.title('Resposta ao Degrau - Erro de Posicionamento')
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(CAMINHO_PLOT + 'error_response.png', dpi=300)

if __name__ == "__main__":
    plot_step_response(CAMINHO_CSV)