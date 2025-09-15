/*const prompt = require('prompt-sync')();

let sudoku = [
    [],
    [],
    [],
    []
]

for (let l = 0; l < 4; l++) {
    for (let c = 0; c < 4; c++) {
        sudoku[l][c] = Number(prompt('Digite o numero na posição: '))
    }
}
console.log(sudoku)
*/

/*
let sudoku = [
    [0, 0, 0, 1],
    [2, 1, 3, 4],
    [0, 0, 0, 0],
    [0, 0, 4, 2],
]
*/
let sudoku = [
  [0,0,0,0],
  [3,0,0,1],
  [0,0,0,0],
  [0,4,0,2]
]

console.log(sudoku)

function verificarLinha(sudoku, linha, numero) {
    // Essa função deve percorrer a linha desejada verificar a existencia do numero na linha
    for (let c = 0; c < 4; c++) {
        if (sudoku[linha][c] === numero) {
            // Verifica se cada numero da array linha é igual ao numero escolhido
            return true
        }
    }
    return false
}

function verificarColuna(sudoku, coluna, numero) {
    for (let l = 0; l < 4; l++) {
        if (sudoku[l][coluna] === numero) {
            return true
        }
    }
    return false
}

function verificarQuadrado(sudoku, linha, coluna, numero) {
    let bloco = []
    // Encontra o bloco e envia para a array bloco
    linha = Math.floor(linha / 2) * 2
    coluna = Math.floor(coluna / 2) * 2
    bloco.push(sudoku[linha][coluna], sudoku[linha][coluna+1], sudoku[linha+1][coluna], sudoku[linha+1][coluna+1], )
    //console.log("Bloco: ", bloco)

    // Percorre o bloco verificando se "numero" existe
    for (let i = 0; i < 4; i++){
        if (bloco[i] === numero) {
            return true
        }
    }
    return false
}


// Backtracking?
function sudokuSolver(sudoku) {
    for (let l = 0; l < 4; l++) {
        for (let c = 0; c < 4; c++) {
            if (sudoku[l][c] === 0) {
                for (let escolha = 1; escolha < 5; escolha++) {
                    if(
                        !verificarLinha(sudoku, l, escolha) && 
                        !verificarColuna(sudoku, c, escolha) && 
                        !verificarQuadrado(sudoku, l, c, escolha)
                    ) {
                        sudoku[l][c] = escolha
                        if (sudokuSolver(sudoku)) { // corta o loop e refaz
                            return true // retorna true caso sudoku acabou
                        }
                        sudoku[l][c] = 0 // refaz caso sudoku a partir daqui caso nao foi resolvido
                    }
                }
                return false
            }
        }
    }
    return true
}

sudokuSolver(sudoku)
console.log(sudoku)

/*
let sudoku = [
    [0, 0, 0, 1],
    [2, 1, 3, 4],
    [0, 0, 0, 0],
    [0, 0, 4, 2],
]
*/