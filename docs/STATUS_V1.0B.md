# PCFX360 — Painel de progresso v1.0b

Este painel separa código existente, validação pendente e funcionalidades futuras. Os números são estimativas de cobertura de engenharia, não percentuais de compatibilidade com jogos.

## Resumo honesto

- Versão registrada: v1.0b.
- Core C++ de host: estrutura existente; workflow atualizado para matriz GCC/Clang e sanitizers; resultados de execução ainda não confirmados.
- CPU V810: subconjunto experimental; não comparar com CPU completa.
- Xbox 360: toolchain e compilação-alvo não comprovadas.
- Artefato `default.xex`: não confirmado/existente.
- Execução de BIOS, vídeo, áudio, CD-ROM e jogos: não implementada de ponta a ponta.

## Métricas aproximadas por subsistema

| Subsistema | Cobertura estimada | Evidência atual | Próxima barreira |
|---|---:|---|---|
| Estrutura CMake/C++ host | 75% | CMake, biblioteca e alvo de testes | Executar CI após v1.0b |
| RAM little-endian | 65% | Operações básicas e testes de limites | Testes exaustivos e mapa PC-FX real |
| Barramento | 35% | Janela RAM configurável, falhas de limites | Regiões mapeadas e MMIO |
| CPU V810 | 15% | Subconjunto de instruções documentado | Auditoria do ISA e PSW por referência independente |
| Testes de CPU | 25% do subconjunto existente | Casos básicos e novos casos de borda | Executar testes, corrigir e ampliar casos |
| Exceções/interrupções | 0–5% | Ainda sem sistema completo | Modelo arquitetural documentado |
| Timers/DMA | 0% | Não implementados | Especificação e dispositivos |
| Vídeo/VDC | 0% | Não implementado | Rasterização e sincronização |
| Áudio | 0% | Não implementado | Modelo de som e mistura |
| CD-ROM/BIOS | 0% | Não implementados | Interface de mídia e carregamento de arquivos do usuário |
| Entrada/controles | 0–5% | Sem fluxo integrado | Mapeamento de comandos |
| UI/menus | 0% | Sem frontend de emulador | Menu, configurações e estado |
| Port Xbox 360/Xenon | 0–5% | Sem build de alvo comprovado | Toolchain, ABI, bibliotecas e linker |
| Empacotamento XEX | 0% | Nenhum `default.xex` validado | Gerar ELF, converter e testar no hardware |
| Compatibilidade de jogos | 0% comprovado | Nenhum jogo demonstrado | Teste com mídia/BIOS fornecidas legalmente pelo usuário |

Percentuais são estimativas qualitativas para priorização, não medidas de código coberto nem promessas de prazo.

## Plano de 1.000 microetapas

A solicitação de 1.000 etapas é organizada em 10 frentes com 100 microetapas planejadas cada. **Planejar as etapas não significa que todas já foram executadas.** A execução deve ser registrada em commits e testes verificáveis.

1. **CPU/ISA — etapas 1–100:** conferir codificação por instrução; criar testes de flags; validar PC; documentar instruções ilegais; comparar com referência independente.
2. **Memória — 101–200:** limites; alinhamento; endianness; acessos 8/16/32-bit; mapa PC-FX com referências.
3. **Barramento/MMIO — 201–300:** regiões; dispatch; registradores de dispositivos; leituras/escritas; erros.
4. **Exceções/interrupções — 301–400:** estado de exceção; vetores; prioridade; máscaras; retorno de exceção.
5. **Timers/DMA — 401–500:** contadores; sincronização; transferência; eventos; interrupções.
6. **Vídeo — 501–600:** registradores VDC; memória de vídeo; camadas/sprites; composição; sincronização.
7. **Áudio — 601–700:** registradores de som; temporização; amostras; mistura; saída.
8. **CD-ROM/BIOS/entrada — 701–800:** comandos de mídia; carregamento de BIOS do usuário; controles; erros de I/O; ciclo de inicialização.
9. **Plataforma Xbox 360 — 801–900:** toolchain; compilação cruzada; ABI; memória/alocação; renderização/áudio de plataforma.
10. **Integração e compatibilidade — 901–1000:** build reproduzível; ELF; XEX; logs; testes no hardware; matriz de compatibilidade.

## Portões obrigatórios de validação

- [ ] GitHub Actions compila e executa CTest com sucesso no host (workflow agora usa GCC, Clang e uma execução com ASan/UBSan).
- [ ] ISA/flags/branches são comparados com documentação técnica ou implementação de referência confiável.
- [ ] Mapa de memória do PC-FX é sustentado por documentação.
- [ ] Nenhum subsistema é declarado concluído apenas porque existe um arquivo ou teste escrito.
- [ ] Toolchain Xbox 360 compila o projeto.
- [ ] `Xenon.elf` é produzido e validado.
- [ ] `default.xex` é produzido e validado.
- [ ] Inicialização é demonstrada em hardware Xbox 360 RGH.
- [ ] BIOS/jogo são testados somente com arquivos obtidos legalmente pelo usuário.

## Avanço paralelo registrado nesta rodada

- [x] Ampliar o workflow de host para GCC e Clang.
- [x] Adicionar job separado com AddressSanitizer e UndefinedBehaviorSanitizer.
- [x] Aplicar avisos de compilador também ao executável de testes.
- [x] Adicionar regressões de barramento em endereços próximos de `0xffffffff` para verificar limites e evitar wraparound.
- [x] Corrigir o barramento para rejeitar acessos que cruzem o limite do espaço de endereços de 32 bits.
- [ ] Confirmar execução dos três jobs e corrigir falhas observadas.

## Commits adicionais desta rodada

- `d6ccdeef`: CI ampliado para GCC, Clang e sanitizers.
- `1d4aa90c`: novos testes de limite do barramento.
- `43b051f6`: avisos de compilação também no alvo de testes.
- `9e233c15`: correção de wraparound no barramento de 32 bits.

O estado do CI consultado não apresentou verificações concluídas; portanto, nenhuma execução é declarada aprovada.

## Próximo avanço recomendado

Antes de expandir a lista de instruções, obter um resultado verde do CI, auditar as instruções e os bits do PSW contra uma referência independente e corrigir qualquer divergência. Só depois seguir para exceções e mapa de memória.
