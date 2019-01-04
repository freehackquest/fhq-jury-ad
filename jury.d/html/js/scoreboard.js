
// post request to server Async
function getAjax (url, callback) {
    callback = callback || function(){};
	var tmpXMLhttp = null;
	if (window.XMLHttpRequest) {
		// code for IE7+, Firefox, Chrome, Opera, Safari
		tmpXMLhttp = tmpXMLhttp || new window.XMLHttpRequest();
	};
	tmpXMLhttp.onreadystatechange=function() {
		if (tmpXMLhttp.readyState==4) {
			if (tmpXMLhttp.responseText == '') {
                obj = { 'result' : 'fail' };
                callback('fail', null);
            } else {
				try {
					var obj = JSON.parse(tmpXMLhttp.responseText);
                    callback(null, obj);
                    delete obj;
				} catch(e) {
					console.error(e.name + ':' + e.message + '\n stack:' + e.stack + '\n' + tmpXMLhttp.responseText);
				}
				delete tmpXMLhttp;
			}
		}
	}
	tmpXMLhttp.open('GET', url, true);
	tmpXMLhttp.send();
};
var scoreboard_content = document.getElementById('scoreboard_content');
var loader_content = document.getElementById('loader_content');

function _animateElement(el, enable) {
    if (el == null) {
        console.error("_animateElement el is null");
        return;
    }
    el.style.animation = enable ? "blinking 0.8s reverse infinite" : '';
}

function updateUIValue(t, teamID, paramName){
    var newValue = '';
    if (paramName == 'score') {
        newValue = t[paramName].toFixed(2);
    } else {
        newValue = '' + t[paramName];
    }
    var el = document.getElementById(paramName + '-' + teamID);
    if (el) {
        var prevVal = el.innerHTML;
        if (prevVal != newValue) {
            document.getElementById(paramName + '-' + teamID).innerHTML = newValue;
            if (paramName == "tries") {
                _animateElement(document.getElementById('tries-icon-' + teamID), true);
            }
        } else {
            if (paramName == "tries") {
                _animateElement(document.getElementById('tries-icon-' + teamID), false);
            }
        }
    } else {
        console.error('Not found element: ' + teamID + '_' + paramName);
    }
};

var inSwitch = false;

function switchUITeamRows(teamID1, teamID2){
    console.log('switchUITeamRows: ' + teamID1 + ' <-> ' + teamID2);
    if (inSwitch) {
    /*    setTimeout(function(teamID1, teamID2){
            switchUITeamRows(teamID1, teamID2);
        },50);*/
        return;
    }
    inSwitch = true;
    var el1 = document.getElementById(teamID1);
    var el2 = document.getElementById(teamID2);

    el1.style.transform = 'translateY(100px)';
    // el2.style.transform = 'translateY(-100px)';
    setTimeout(function(){
        el2.parentNode.insertBefore(el2, el1);
        el1.style.transform = '';
        el2.style.transform = '';
        inSwitch = false;
    },400);
}

function updateScoreboard() {
    getAjax('/api/v1/scoreboard', function(err, resp){
        if (err) {
            scoreboard_content.style.display = 'none';
            loader_content.style.display = 'block';
            return;
        }
        var teamIDs = [];
        for(var teamID in resp){
            var t = resp[teamID];
            teamIDs.push(teamID);
            updateUIValue(t, teamID, 'place');
            updateUIValue(t, teamID, 'score');
            updateUIValue(t, teamID, 'tries');
            for(var sService in t.services){
                var newState = t.services[sService]['status'];
                var newAttack = t.services[sService]['attack'];
                var newDefence = t.services[sService]['defence'];
                var newSLA = t.services[sService]['uptime'];
                var elId = 'status-' + teamID + '-' + sService;
                var el = document.getElementById(elId);
                if (el != null) {
                    if (!el.classList.contains(newState)) {
                        el.classList.remove('up');
                        el.classList.remove('down');
                        el.classList.remove('mumble');
                        el.classList.remove('corrupt');
                        el.classList.remove('shit');
                        el.classList.add(newState);
                    }
                } else {
                    console.error(elId + '- not found');
                }
                var sCell = teamID + '-' + sService;
                console.log(sCell);
                var elAtt = document.getElementById('att-' + sCell);
                if (elAtt != null) elAtt.innerHTML = newAttack; else console.error('att-' + sCell + ' - not found');
                var elDef = document.getElementById('def-' + sCell);
                if (elDef != null) elDef.innerHTML = newDefence; else console.error('def-' + sCell + ' - not found');
                var elUptime = document.getElementById('uptime-' + sCell);
                if (elUptime != null) elUptime.innerHTML = newSLA.toFixed(2) + '%'; else console.error('uptime-' + sCell + ' - not found');
            }
        }

        // sort by places
        var elms2 = [];
        var elms = document.getElementsByClassName('tm');
        for(var i = 0; i < elms.length; i++){
            var el1 = elms[i];
            var place1 = parseInt(resp[el1.id]['place'], 10);
            elms2.push({
                e: elms[i],
                p: place1
            });
        }
        elms2.sort(function(a, b) {
            return a.p - b.p;
        });
        for(var i = 0; i < elms2.length; i++){
            elms2[i].e.style.top = ((i+1)*70) + 'px';
        }
        var bSorted = false;

        while (!bSorted){
            bSorted = true;
            
        }

        // open controls
        if (scoreboard_content.style.display != 'block') {
            scoreboard_content.style.display = 'block'
        }
        if (loader_content.style.display != 'none') {
            loader_content.style.display = 'none';
        }
    });
}

// init scoreboard
getAjax('/api/v1/game', function(err, resp){
    if (err) {
        console.error("Problem with game info ", err);
        return;
    }
    document.getElementById('game_name').innerHTML = resp.game_name;
    document.getElementById('game_time_range').innerHTML = resp.game_start + ' - ' + resp.game_end;
    
    console.log(resp);

    // generate teams-services table
    var sContent = 
    "<div class='scoreboard' id='table_scoreboard'>"
    + "    <div class='hdrs'>"
    + "        <div class='place'>#</div>"
    + "        <div class='team-logo'></div>"
    + "        <div class='team'>Team</div>"
    + "        <div class='score'>Score</div>";
    for (var i = 0; i < resp.services.length; i++) {
        sContent += "<div class='service'>" + resp.services[i].name + "<br><small>(service)</small></div>";
    }
    sContent += ''
        + '        <div class="activity">Activity</div>'
        + '  </div>';

    var sTeamListSelect = '';

    for (var iteam = 0; iteam < resp.teams.length; iteam++) {
        var sTeamId = resp.teams[iteam].id;
        sTeamListSelect += '<option value=' + sTeamId + '>' + sTeamId + '</option>';
        sContent += ""
            + "<div class='tm' id='" + sTeamId + "'>"
            + '  <div class="place" id="place-' + sTeamId + '" >' + (iteam + 1) + '</div>'
            + "  <div class='team-logo'><img class='team-logo' src='" + resp.teams[iteam].logo + "'/></div>"
            + "  <div class='team'>"
            + "    <div class='team-name'>" + resp.teams[iteam].name + "</div>"
            + "    <div class='team-ip'> id: " + sTeamId + ", ip: " + resp.teams[iteam].ip_address + "</div>"
            + "  </div>"
            + "  <div class='score' id='score-" + sTeamId + "'>0</div>";

        for (var i = 0; i < resp.services.length; i++) {
            var sServiceID = resp.services[i].id;
            sContent += ""
            + "<div class='service'>"
            + "  <div class='service-status down' id='status-" + sTeamId +  "-" + sServiceID + "'> "
            + '   <div class="service-att-def">'
            + '       <div class="d-icn def" id="def-' + sTeamId +  '-' + sServiceID + '">0</div>'
            + ' | '
            + '       <div class="d-icn att" id="att-' + sTeamId +  '-' + sServiceID + '">0</div>'
            + '   </div>'
            + '   <div class="service-sla d-icn upt" id="uptime-' + sTeamId +  '-' + sServiceID + '">0.0%</div>'
            + "  </div>"
            + "</div>\n";
        }
        sContent += ""
            + '   <div class="activity">'
            + '      <div class="activity-value" id="tries-' + sTeamId +  '">0</div>'
            + '      <div class="activity-icon" id="tries-icon-' + sTeamId +  '"></div>'
            + '   </div>'
            + "</div>";
    }
    sContent += "</div>";

    document.getElementById('scoreboard_content').innerHTML = sContent;
    document.getElementById('team_list').innerHTML = sTeamListSelect;

    updateScoreboard();

    // start poling
    setInterval(function(){
        updateScoreboard()
    }, 3000);
});
