
class Cell{
    constructor(x,y,type, selected){
        this.x = x;
        this.y = y;
        this.type = type;
        this.selected = selected;
    }
}

class Player{
    constructor(x,y,us){
        this.x = x;
        this.y = y;
        this.us = us;
    }
}

class Element{
    constructor(w,h,t,l){
        this.w = w;
        this.h = h;
        this.t = t;
        this.l = l;
    }
}

const canvas = document.getElementById('canvas'); // canvas
const ctx = canvas.getContext('2d'); // canvas context

const gridSize = 16; // size of grid in cells (grid is square)
const innerCellMarginps = 0.5; // canvas size's persents% between cells
let cellMarginpx; // current size between cells in pixels
let grid = []; // two-dimentional array of cells

let lastSessionState; // last state response from server

let playerUs = new Player(0,0, true); // our player object
let playerEnemy = new Player(0,0, false); // enemy player object

let selectedCell; // highlighted celected cell

// creates cells for grid
function initGrid(){
    for(x = 0; x < gridSize; ++x){
        let col = [];
        for(y = 0; y < gridSize; ++y){
            col.push(new Cell(x,y,"grass", false));
        }
        grid.push(col);
    }
}

// returns cell rectangle to draw
function elementData(x, y){
    let res = new Element(0,0,0,0);

    res.w = canvas.width/gridSize;
    res.h = canvas.height/gridSize;
    res.t = y * res.h;
    res.l = x * res.w;

    res.t += cellMarginpx;
    res.l += cellMarginpx;
    res.w -= cellMarginpx * 2;
    res.h -= cellMarginpx * 2;

    return res;
}

//draws one cell
function drawCell(cell){
    const element = elementData(cell.x, cell.y);
    switch(cell.type){
        case "grass":
            ctx.fillStyle = "rgb("+(79+cell.selected*50)+" 255 170)";
            break;
        case "wall":
            ctx.fillStyle = "rgb(191 255 252)";
            break;
    }
    ctx.fillRect(element.l, element.t, element.w, element.h);
}

// draws player
function drawPlayer(player){
    const element = elementData(player.x, player.y);
    if(player.us)
        ctx.fillStyle = "rgb(66 79 255)";
    else
        ctx.fillStyle = "rgb(255 145 145)";
    
    ctx.fillRect(element.l, element.t, element.w, element.h);
}

// draws all players
function drawPlayers(){
    drawPlayer(playerUs);
    drawPlayer(playerEnemy);
}

// draws all cells in grid
function drawGrid(){
    cellMarginpx = canvas.width * innerCellMarginps / 100;

    for(y = 0; y < gridSize; ++y){
        for (x = 0; x < gridSize; ++x){
            drawCell(grid[x][y]);
        }
    }
}

// draws all objects on scene
function drawScene(){
    drawGrid();
    drawPlayers();
}

// makes given cell a wall
function makeWall(cell){
    cell.type = "wall";
}

// updates all obstacle objects by server's data
function updateTerrain(terrain){
    for(i = 0; i < terrain.length; ++i){
        let block = terrain[i];
        makeWall(grid[block.posX][block.posY]);
    }
}

// updates all player objects by server's data
function updatePlayers(players){
    let dataUs, dataEnemy;
    if (players[0].login == localStorage.getItem('login')){
        dataUs = players[0];
        dataEnemy = players[1];
    }
    else{
        dataUs = players[1];
        dataEnemy = players[0];
    }
    playerUs.x = dataUs.posX;
    playerUs.y = dataUs.posY;

    playerEnemy.x = dataEnemy.posX;
    playerEnemy.y = dataEnemy.posY;
}

// loads and updates information from the server immediately
async function loadScene(){
    const stateResponse = await 
        fetch('http://localhost:9999/api/game/session_state?sessionId='+localStorage.getItem('sessionId'), {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json',
                'Authorization':'Bearer ' + localStorage.getItem('token')
            },
        });
    lastSessionState = await stateResponse.json();
    updateScene();
}

// updates all objects on scene
function updateScene(){
    const terrain = lastSessionState.terrain;
    const players = lastSessionState.players;
    updateTerrain(terrain);
    updatePlayers(players);
    drawScene();
}
     
// event for resizing canvas to keep with normal size
async function resizeCanvas() {
    canvas.width = parseFloat(getComputedStyle(canvas).width);
    canvas.height = parseFloat(getComputedStyle(canvas).height);

    drawScene();
}

// returns mouse position in the canvas
function getMousePos(canvas, event) {
    var rect = canvas.getBoundingClientRect();
    return {
        x: event.clientX - rect.left,
        y: event.clientY - rect.top,
    };
}

// returns cell which is mouse hovered on
function getCell(coordinates) {
    const x = Math.floor(coordinates.x/(canvas.width/gridSize));
    const y = Math.floor(coordinates.y/(canvas.height/gridSize));
    const fit = x >= 0 && x < gridSize && y >= 0 && y < gridSize;
    if(fit)
        return grid[x][y];
    return null;
}

// click event
function onClick(event) {
    walk();
    playerUs.x = selectedCell.x;
    playerUs.y = selectedCell.y;
    drawScene();
}

// drops selected cell to null
function dropSelectedCell(){
    if(selectedCell){
        selectedCell.selected = false;
        selectedCell = null;
    }
}

// mouse hovering event
function onMouseOver(event){
    dropSelectedCell();
    const mouse = getMousePos(canvas, event);
    const cell = getCell(mouse);
    if (!cell) return;
    cell.selected = true;
    selectedCell = cell;
    drawScene();
}

// mouse out event
function onMouseOut(event){
    dropSelectedCell();
    drawScene();
}

// asyncronically waits for opponent move
function waitForStateChange() {
    fetch('http://localhost:9999/api/game/session_state_change?sessionId='+localStorage.getItem('sessionId'), 
    {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
    }).then(response=>{
        if (!response.ok)
            throw "response in non-ok";
        return response.json();
    }).then(json=>{
        lastSessionState = json;
        waitForStateChange();
        updateScene();
    });
}

function walk(){
    const data = {
        move_type: "move",
        posX: selectedCell.x,
        posY: selectedCell.y
    };

    move(data);
}

// sends player's moving
function move(data){
    fetch('http://localhost:9999/api/game/move?sessionId='+localStorage.getItem('sessionId'), 
    {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
        body: JSON.stringify(data)
    }).then(response=>{
        updateScene();
        return response.json();
    }).then(json=>{
        console.log(json);
    });
}

initGrid();
loadScene();
resizeCanvas();
waitForStateChange();

window.addEventListener('resize', resizeCanvas, false);
canvas.addEventListener('click', onClick, false);
canvas.addEventListener('mousemove', onMouseOver, false);
canvas.addEventListener('mouseout', onMouseOut, false);
  