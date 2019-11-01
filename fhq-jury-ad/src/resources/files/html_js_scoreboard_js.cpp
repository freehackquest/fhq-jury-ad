// automaticly generated by update_resources.py

#include <html_js_scoreboard_js.h>

REGISTRY_RESOURCE_FILE(RES_html_js_scoreboard_js)

const std::string &RES_html_js_scoreboard_js::filename() {
    static const std::string s = "html/js/scoreboard.js";
    return s;
}

const int RES_html_js_scoreboard_js::bufferSize() {
    return 11353;
}

const char *RES_html_js_scoreboard_js::buffer() {
    static const std::string sRet =  // size: 11353
        "\n"
        "// post request to server Async\n"
        "function getAjax (url, callback) {\n"
        "    callback = callback || function(){};\n"
        "	var tmpXMLhttp = null;\n"
        "	if (window.XMLHttpRequest) {\n"
        "		// code for IE7+, Firefox, Chrome, Opera, Safari\n"
        "		tmpXMLhttp = tmpXMLhttp || new window.XMLHttpRequest();\n"
        "	};\n"
        "	tmpXMLhttp.onreadystatechange=function() {\n"
        "		if (tmpXMLhttp.readyState==4) {\n"
        "			if (tmpXMLhttp.responseText == '') {\n"
        "                obj = { 'result' : 'fail' };\n"
        "                callback('fail', null);\n"
        "            } else {\n"
        "				try {\n"
        "					var obj = JSON.parse(tmpXMLhttp.responseText);\n"
        "                    callback(null, obj);\n"
        "                    delete obj;\n"
        "				} catch(e) {\n"
        "					console.error(e.name + ':' + e.message + '\\n stack:' + e.stack + '\\n' + tmpXMLhttp.responseText);\n"
        "				}\n"
        "				delete tmpXMLhttp;\n"
        "			}\n"
        "		}\n"
        "	}\n"
        "	tmpXMLhttp.open('GET', url, true);\n"
        "	tmpXMLhttp.send();\n"
        "};\n"
        "var scoreboard_content = document.getElementById('scoreboard_content');\n"
        "var loader_content = document.getElementById('loader_content');\n"
        "\n"
        "function _animateElement(el, enable) {\n"
        "    if (el == null) {\n"
        "        console.error(\"_animateElement el is null\");\n"
        "        return;\n"
        "    }\n"
        "    el.style.animation = enable ? \"blinking 0.8s reverse infinite\" : '';\n"
        "}\n"
        "\n"
        "function _animateElementOneTime(el) {\n"
        "    if (el == null) {\n"
        "        console.error(\"_animateElementOneTime el is null\");\n"
        "        return;\n"
        "    }\n"
        "\n"
        "    el.style.animation = \"fastblinking 0.8s reverse infinite\";\n"
        "    var i = setTimeout(function() {\n"
        "        el.style.animation = '';\n"
        "        clearTimeout(i);\n"
        "    },800);\n"
        "}\n"
        "\n"
        "function silentUpdate(elid, newValue) {\n"
        "    var el = document.getElementById(elid)\n"
        "    if (!el) {\n"
        "        console.error(\"Not found element with id \" + elid);\n"
        "        return;\n"
        "    }\n"
        "    if (el.innerHTML != newValue) {\n"
        "        el.innerHTML = newValue;\n"
        "        _animateElementOneTime(el);\n"
        "        // TODO make simple anim\n"
        "    }\n"
        "}\n"
        "\n"
        "\n"
        "function updateUIValue(t, teamID, paramName){\n"
        "    var newValue = '';\n"
        "    if (paramName == 'points') {\n"
        "        newValue = t[paramName].toFixed(1);\n"
        "    } else {\n"
        "        newValue = '' + t[paramName];\n"
        "    }\n"
        "    var el = document.getElementById(paramName + '-' + teamID);\n"
        "    if (el) {\n"
        "        var prevVal = el.innerHTML;\n"
        "        if (prevVal != newValue) {\n"
        "            document.getElementById(paramName + '-' + teamID).innerHTML = newValue;\n"
        "            if (paramName == \"tries\") {\n"
        "                _animateElement(document.getElementById('tries-icon-' + teamID), true);\n"
        "            }\n"
        "        } else {\n"
        "            if (paramName == \"tries\") {\n"
        "                _animateElement(document.getElementById('tries-icon-' + teamID), false);\n"
        "            }\n"
        "        }\n"
        "    } else {\n"
        "        console.error('Not found element: ' + teamID + '_' + paramName);\n"
        "    }\n"
        "};\n"
        "\n"
        "var inSwitch = false;\n"
        "\n"
        "function switchUITeamRows(teamID1, teamID2){\n"
        "    console.log('switchUITeamRows: ' + teamID1 + ' <-> ' + teamID2);\n"
        "    if (inSwitch) {\n"
        "    /*    setTimeout(function(teamID1, teamID2){\n"
        "            switchUITeamRows(teamID1, teamID2);\n"
        "        },50);*/\n"
        "        return;\n"
        "    }\n"
        "    inSwitch = true;\n"
        "    var el1 = document.getElementById(teamID1);\n"
        "    var el2 = document.getElementById(teamID2);\n"
        "\n"
        "    el1.style.transform = 'translateY(100px)';\n"
        "    // el2.style.transform = 'translateY(-100px)';\n"
        "    setTimeout(function(){\n"
        "        el2.parentNode.insertBefore(el2, el1);\n"
        "        el1.style.transform = '';\n"
        "        el2.style.transform = '';\n"
        "        inSwitch = false;\n"
        "    },400);\n"
        "}\n"
        "\n"
        "function updateScoreboard() {\n"
        "    getAjax('/api/v1/scoreboard', function(err, resp){\n"
        "        if (err) {\n"
        "            scoreboard_content.style.display = 'none';\n"
        "            loader_content.style.display = 'block';\n"
        "            return;\n"
        "        }\n"
        "        console.log(resp);\n"
        "        for (var serviceId in resp.s_sta) {\n"
        "            var s = resp.s_sta[serviceId]\n"
        "            silentUpdate(serviceId + '-first-blood', s.first_blood);\n"
        "            silentUpdate(serviceId + '-cost-att', s.cost_att.toFixed(1));\n"
        "            silentUpdate(serviceId + '-cost-def', s.cost_def.toFixed(1));\n"
        "            silentUpdate(serviceId + '-all-flags-att', s.af_att)\n"
        "            silentUpdate(serviceId + '-all-flags-def', s.af_def)\n"
        "        }\n"
        "        var teamIDs = [];\n"
        "        for(var teamID in resp.scoreboard){\n"
        "            var t = resp.scoreboard[teamID];\n"
        "            teamIDs.push(teamID);\n"
        "            silentUpdate(teamID + '-points', t.points.toFixed(1));\n"
        "            updateUIValue(t, teamID, 'place');\n"
        "            // updateUIValue(t, teamID, 'points');\n"
        "            updateUIValue(t, teamID, 'tries');\n"
        "            for(var sService in t.ts_sta){\n"
        "                var newState = t.ts_sta[sService]['status'];\n"
        "                var newAttackFlags = t.ts_sta[sService]['att'];\n"
        "                var newDefenceFlags = t.ts_sta[sService]['def'];\n"
        "                var newAttackPoints = t.ts_sta[sService]['pt_att'];\n"
        "                var newDefencePoints = t.ts_sta[sService]['pt_def'];\n"
        "                var newSLA = t.ts_sta[sService]['upt'];\n"
        "                var elId = 'status-' + teamID + '-' + sService;\n"
        "                var el = document.getElementById(elId);\n"
        "                if (el != null) {\n"
        "                    if (!el.classList.contains(newState)) {\n"
        "                        el.classList.remove('up');\n"
        "                        el.classList.remove('down');\n"
        "                        el.classList.remove('mumble');\n"
        "                        el.classList.remove('corrupt');\n"
        "                        el.classList.remove('shit');\n"
        "                        el.classList.remove('wait');\n"
        "                        el.classList.remove('coffeebreak');\n"
        "                        el.classList.add(newState);\n"
        "                        _animateElementOneTime(el);\n"
        "\n"
        "                    }\n"
        "                } else {\n"
        "                    console.error(elId + '- not found');\n"
        "                }\n"
        "                var sCell = teamID + '-' + sService;\n"
        "                console.log(sCell);\n"
        "                silentUpdate('att-' + sCell, newAttackFlags)\n"
        "                silentUpdate('def-' + sCell, newDefenceFlags)\n"
        "                silentUpdate('pt_att-' + sCell, newAttackPoints)\n"
        "                silentUpdate('pt_def-' + sCell, newDefencePoints)\n"
        "            }\n"
        "        }\n"
        "\n"
        "        // sort by places\n"
        "        var elms2 = [];\n"
        "        var elms = document.getElementsByClassName('tm');\n"
        "        for(var i = 0; i < elms.length; i++){\n"
        "            var el1 = elms[i];\n"
        "            var place1 = parseInt(resp[\"scoreboard\"][el1.id]['place'], 10);\n"
        "            elms2.push({\n"
        "                e: elms[i],\n"
        "                p: place1\n"
        "            });\n"
        "        }\n"
        "        elms2.sort(function(a, b) {\n"
        "            return a.p - b.p;\n"
        "        });\n"
        "        for(var i = 0; i < elms2.length; i++){\n"
        "            elms2[i].e.style.top = (50 + (i+1)*70) + 'px';\n"
        "        }\n"
        "        var bSorted = false;\n"
        "\n"
        "        while (!bSorted){\n"
        "            bSorted = true;\n"
        "        }\n"
        "\n"
        "        // open controls\n"
        "        if (scoreboard_content.style.display != 'block') {\n"
        "            scoreboard_content.style.display = 'block'\n"
        "        }\n"
        "        if (loader_content.style.display != 'none') {\n"
        "            loader_content.style.display = 'none';\n"
        "        }\n"
        "    });\n"
        "}\n"
        "\n"
        "function formatGameTimings(periods) {\n"
        "    // TODO beauty print periods\n"
        "}\n"
        "\n"
        "// init scoreboard\n"
        "getAjax('/api/v1/game', function(err, resp){\n"
        "    if (err) {\n"
        "        console.error(\"Problem with game info \", err);\n"
        "        return;\n"
        "    }\n"
        "    document.getElementById('game_name').innerHTML = resp.game_name;\n"
        "\n"
        "    // TODO beauty print periods \n"
        "    if (resp.game_has_coffee_break) {\n"
        "        document.getElementById('game_time_range').innerHTML = \n"
        "            resp.game_start + ' - ' + resp.game_coffee_break_start + ' (coffee break) '\n"
        "            + resp.game_coffee_break_end + ' - ' + resp.game_end;\n"
        "    } else {\n"
        "        document.getElementById('game_time_range').innerHTML = resp.game_start + ' - ' + resp.game_end;\n"
        "    }\n"
        "\n"
        "    console.log(resp);\n"
        "\n"
        "    // generate teams-services table\n"
        "    var sContent = \n"
        "    \"<div class='scoreboard' id='table_scoreboard'>\"\n"
        "    + \"    <div class='hdrs'>\"\n"
        "    + \"        <div class='place'>#</div>\"\n"
        "    + \"        <div class='team-logo'></div>\"\n"
        "    + \"        <div class='team'>Team</div>\"\n"
        "    + \"        <div class='score'>Points</div>\";\n"
        "    for (var i = 0; i < resp.services.length; i++) {\n"
        "        var serviceId = resp.services[i].id\n"
        "        sContent += \"<div class='service'><b>\" + resp.services[i].name + \" </b>\"\n"
        "        + \"<small>[r:\" + resp.services[i].round_time_in_sec + \"s]</small>\"\n"
        "        + \"<div>\"\n"
        "        + \"     <div class='text green points' id='\" + serviceId + \"-cost-def'>0.0</div> \"\n"
        "        + \"     | <div class='text red points' id='\" + serviceId + \"-cost-att'>0.0</div> \"\n"
        "        + \"</div>\"\n"
        "        + \"<div class='service-att-def'>\"\n"
        "        + \"     <div class='d-icn def' id='\" + serviceId + \"-all-flags-def'>0</div> \"\n"
        "        + \"     | <div class='d-icn att' id='\" + serviceId + \"-all-flags-att'>0</div>\"\n"
        "        + \"</div>\"\n"
        "        + \"<div>\"\n"
        "        + \"    <div class='first-blood' id='\" + serviceId + \"-first-blood'>???</div>\"\n"
        "        + \"</div>\"\n"
        "        + \"</div>\";\n"
        "    }\n"
        "    sContent += ''\n"
        "        + '        <div class=\"activity\">Activity</div>'\n"
        "        + '  </div>';\n"
        "\n"
        "    var sTeamListSelect = '';\n"
        "\n"
        "    for (var iteam = 0; iteam < resp.teams.length; iteam++) {\n"
        "        var sTeamId = resp.teams[iteam].id;\n"
        "        sTeamListSelect += '<option value=' + sTeamId + '>' + sTeamId + '</option>';\n"
        "        sContent += \"\"\n"
        "            + \"<div class='tm' id='\" + sTeamId + \"'>\"\n"
        "            + '  <div class=\"place\" id=\"place-' + sTeamId + '\" >' + (iteam + 1) + '</div>'\n"
        "            + \"  <div class='team-logo'><img class='team-logo' src='\" + resp.teams[iteam].logo + \"'/></div>\"\n"
        "            + \"  <div class='team'>\"\n"
        "            + \"    <div class='team-name'>\" + resp.teams[iteam].name + \"</div>\"\n"
        "            + \"    <div class='team-ip'> id: \" + sTeamId + \", ip: \" + resp.teams[iteam].ip_address + \"</div>\"\n"
        "            + \"  </div>\"\n"
        "            + \"  <div class='score'><div class='points' id='\" + sTeamId + \"-points'>0</div></div>\";\n"
        "\n"
        "        for (var i = 0; i < resp.services.length; i++) {\n"
        "            var sServiceID = resp.services[i].id;\n"
        "            sContent += \"\"\n"
        "            + \"<div class='service'>\"\n"
        "            + \"  <div class='service-status down' id='status-\" + sTeamId +  \"-\" + sServiceID + \"'> \"\n"
        "            + '   <div class=\"service-att-def\">'\n"
        "            + '       <div class=\"d-icn def\" id=\"def-' + sTeamId +  '-' + sServiceID + '\">0</div>'\n"
        "            + ' | '\n"
        "            + '       <div class=\"d-icn att\" id=\"att-' + sTeamId +  '-' + sServiceID + '\">0</div>'\n"
        "            + '   </div>'\n"
        "            + '   <div>'\n"
        "            + '     <div class=\"text green points\" id=\"pt_def-' + sTeamId +  '-' + sServiceID + '\">0.0</div> '\n"
        "            + '     | <div class=\"text red points\" id=\"pt_att-' + sTeamId +  '-' + sServiceID + '\">0.0</div> '\n"
        "            + '   </div>'\n"
        "            + '  </div>'\n"
        "            + \"</div>\\n\";\n"
        "        }\n"
        "        sContent += \"\"\n"
        "            + '   <div class=\"activity\">'\n"
        "            + '      <div class=\"activity-value\" id=\"tries-' + sTeamId +  '\">0</div>'\n"
        "            + '      <div class=\"activity-icon\" id=\"tries-icon-' + sTeamId +  '\"></div>'\n"
        "            + '   </div>'\n"
        "            + \"</div>\";\n"
        "    }\n"
        "    sContent += \"</div>\";\n"
        "\n"
        "    document.getElementById('scoreboard_content').innerHTML = sContent;\n"
        "    document.getElementById('team_list').innerHTML = sTeamListSelect;\n"
        "\n"
        "    updateScoreboard();\n"
        "\n"
        "    // start poling\n"
        "    setInterval(function(){\n"
        "        updateScoreboard()\n"
        "    }, 3000);\n"
        "});\n"
    ;
    return sRet.c_str();
} //::buffer() 

