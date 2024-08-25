
// sends player's moving
function move(data){
    fetch('http://' + IPADDR + '/api/game/move?sessionId='+sessionId, 
    {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
        body: JSON.stringify(data)
    });
}

function walk(){
    const data = {
        move_type: "walk",
        X: selectedCell.X,
        Y: selectedCell.Y
    };
    if(!now_turn)
        return false;
    if(Math.abs(data.X - playerUs.X) + Math.abs(data.Y - playerUs.Y) != 1)
        return false;

    move(data);
    return true;
}

function placeBomb(){
    const data = {
        move_type: "place_bomb",
        X: selectedCell.X,
        Y: selectedCell.Y
    };
    if(!now_turn)
        return false;
    if( Math.abs(data.X - playerUs.X) > 1 &&
        Math.abs(data.Y - playerUs.Y) > 1)
        return false;

    move(data);
    return true;
}

function resign(){
    move({ move_type: "resign" });
}

// asyncronically waits for opponent move
function waitForStateChange() {
    fetch('http://' + IPADDR + '/api/game/session_state_change?sessionId='+sessionId+"&from_move="+last_move, 
    {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
    }).then(response=>{
        if (!response.ok)
            window.location.href = 'http://' + IPADDR + '/session_state.html?sessionId='+sessionId;
        return response.json();
    }).then(async json=>{
        await handleEvents(json);
        waitForStateChange();
    });
}