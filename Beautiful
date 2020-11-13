import React from 'react'
import render from 'react-hyperscript'
import buildHyperscripts from 'hyperscript-helpers'
import './App.css'

{ button, div, ol, li } = buildHyperscripts render

Square = ({ onClick, value }) -> 
  button '.square', { onClick }, value

class Board extends React.Component
  renderSquare: (i) => 
    squareProps =
      value: @props.squares[i]
      onClick: () => @props.onClick(i)
    render Square, squareProps
  
  render: =>
    row1 = div '.board-row .1', [@renderSquare(0), @renderSquare(1), @renderSquare(2)]
    row2 = div '.board-row .2', [@renderSquare(3), @renderSquare(4), @renderSquare(5)]
    row3 = div '.board-row .3', [@renderSquare(6), @renderSquare(7), @renderSquare(8)]
    div '#board', [ row1, row2, row3 ]
  
class Game extends React.Component 
  constructor: (props) ->
    super props
    @state = 
      history: [{ squares: Array(9).fill(null) }]
      stepNumber: 0
      xIsNext: true
  

  handleClick: (i) => 
    history = @state.history.slice 0, (@state.stepNumber + 1)
    current = history[history.length - 1]
    squares = current.squares.slice()

    if calculateWinner squares or squares[i] then return

    squares[i] = if @state.xIsNext then 'X' else 'O'
    @setState {
      history: history.concat [
        {
          squares: squares
        }
      ]
      stepNumber: history.length
      xIsNext: not @state.xIsNext
    }
  

  jumpTo: (step) => 
    @setState {
      stepNumber: step,
      xIsNext: (step % 2) == 0
    }
  

  render: () =>
    history =@state.history
    current = history[@state.stepNumber]
    winner = calculateWinner current.squares

    moves = history.map (step, move) =>
      desc = if move then 'Go to move #' + move else 'Go to game start'
      historyMoveButton = button { onClick: () => @jumpTo move }, desc
      li { key: move }, historyMoveButton

    status = if winner then 'Winner: ' + winner else 'Next player: ' + if @state.xIsNext then 'X' else 'O'
    gameStatus = div '#status', status
    gameMoves = ol '#moves', moves
    gameBoard = div '.game-board', [ render Board, { squares: current.squares, onClick: (i) => @handleClick i } ]
    gameInfo = div '.game-info', [
      gameStatus,
      gameMoves
    ]
    div '.game', [
      gameBoard,
      gameInfo
    ]
  


calculateWinner = (squares) ->
  lines = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [2, 4, 6]
  ]
  i = 0
  while i < lines.length
    [a, b, c] = lines[i]
    if squares[a] and squares[a] == squares[b] and squares[a] == squares[c]
      squares[a]
    i++
  null


export default Game
