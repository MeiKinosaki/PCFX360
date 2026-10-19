# PCFX360

Emulador experimental de NEC PC-FX para Xbox 360 (Xenon), com desenvolvimento incremental.

## Estado atual — v1.0b

Esta versão estabelece a estrutura inicial compilável do projeto, testes de memória e integração contínua para compilação no host. **Ainda não é um emulador jogável e não produz `default.xex`.**

### Componentes
- Modelo inicial de memória RAM e acessos little-endian.
- Estado arquitetural e subconjunto experimental de instruções da CPU NEC V810.
- Testes unitários básicos para memória.
- CMake e GitHub Actions para validar a compilação em Linux.
- Subconjunto experimental V810 com instruções imediatas, desvios condicionais, operações de leitura/escrita de memória, SUB/CMP/AND/XOR/NOT/ANDI/XORI/SHL/SHR/SAR e testes básicos.
- v1.0b: testes de regressão adicionais para r0, busca truncada de instrução de 32 bits, acesso não mapeado e deslocamento com contagem zero. Os testes foram adicionados, mas ainda precisam de execução confirmada no CI.
- Roadmap documentando os subsistemas ainda pendentes.

## Compilar e testar no computador

```sh
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Limitações conhecidas
- CPU V810: execução experimental de MOV, ADD e OR (registrador–registrador), ADDI, MOVEA, ORI e MOVHI (imediatas) desvios condicionais Format III, LD.B/LD.H/LD.W/ST.B/ST.H/ST.W e SUB/CMP/AND/XOR/NOT/ANDI/XORI/SHL/SHR/SAR; ainda insuficiente para executar jogos.
- Mapa completo de memória, timers, interrupções e periféricos: pendentes.
- VDC, vídeo, áudio, CD-ROM, BIOS e controles: pendentes.
- Toolchain Xbox 360, integração com SDK e geração de `default.xex`: pendentes.
- Nenhuma validação em hardware Xbox 360 foi feita.

Consulte `docs/ROADMAP.md` para a sequência planejada. v1.0b amplia a cobertura de testes, mas não representa validação independente da CPU nem um emulador jogável. Não inclua BIOS, firmware ou imagens de jogos protegidos por direitos autorais no repositório.
