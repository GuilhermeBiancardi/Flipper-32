function request(url, data, success = "", error = "") {
    var request = $.ajax({
        url: url,
        method: "GET",
        data: data,
    });

    request.done(function (response) {
        if (response) {
            if(success != "") {
                success(response);
            }
        } else {
            if(error != "") {
                error(response);
            }
        }
    });

    request.fail(function (response) {
        error(response);
    });
}

function loadPage(obj) {

    var page_load = $(obj).attr("id");

    if (page_load) {

        request("modules/pages/" + page_load + "/" + page_load + ".html", {}, function(response) {
            $("#modules").html(response);
            EventsReload();
            popover_elements();
        }, function(response) {
            // console.log(response);
        });

    }

}

function EventsReload() {

    $(".menu-toggler").unbind();
    $(".menu-toggler").click(function () {
        $("body").toggleClass("menu-toggler-on");
    });

    $('.toast-button').unbind();
    $('.toast-button').click(function () {
        var id = $(this).attr("id").split("_");
        $("#toast-" + id[1]).toast('show');
    });

    $(".menu-link").unbind();
    $(".menu-link").click(function () {
        loadPage($(this));
    });

}

function isMobile() {
    if ($(window).width() < 800) {
        $('body').addClass('app-mobile');
        $('body').addClass('menu-toggler-on');
    } else {
        $('body').removeClass('app-mobile');
        $('body').removeClass('menu-toggler-on');
    }
}

function notify(type, mensage) {
    if (type == "success") {
        $("#toast-success-mensage").html(mensage);
        $("#toast-success").toast("show");
    } else if (type == "error") {
        $("#toast-error-mensage").html(mensage);
        $("#toast-error").toast("show");
    } else if (type == "info") {
        $("#toast-info-mensage").html(mensage);
        $("#toast-info").toast("show");
    } else if (type == "warning") {
        $("#toast-warning-mensage").html(mensage);
        $("#toast-warning").toast("show");
    }
}

function isJson(str) {
    try {
        JSON.parse(str);
    } catch (e) {
        return false;
    }
    return true;
}

function HexToString(hexx) {
    var hex = hexx.toString();
    var str = '';
    for (var i = 0; i < hex.length; i += 2)
        str += String.fromCharCode(parseInt(hex.substr(i, 2), 16));
    return str;
}

function StringToHex(str) {
    let hex = '';
    for (let i = 0; i < str.length; i++) {
        let charCode = str.charCodeAt(i).toString(16);
        hex += (charCode.length === 1 ? '0' + charCode : charCode);
    }
    return hex.toUpperCase();
}

function remove_accents(str) {

    com_acento = "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝŔÞßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþÿŕ.()[]{}/-";
    sem_acento = "AAAAAAACEEEEIIIIDNOOOOOOUUUUYRsBaaaaaaaceeeeiiiionoooooouuuuybyr_________";
    novastr = "";
    for(i = 0; i < str.length; i++) {
        troca=false;
        for (a = 0; a < com_acento.length; a++) {
            if (str.substr(i,1)==com_acento.substr(a,1)) {
                novastr+=sem_acento.substr(a,1);
                troca=true;
                break;
            }
        }
        if (troca==false) {
            novastr+=str.substr(i,1);
        }
    }
    return novastr;
}

function generateTreeView(id, json, nivel = 0, path = "") {
    var size = 25.75;
    for (var i = 0; i < json.length; i++) {
        var item = path + json[i].text;
        var item_id = remove_accents(item.replaceAll(/\ |\\/g, "_").toLowerCase());
        if (json[i].hasOwnProperty("nodes")) {
            if (json[i].nodes.length > 0) {
                $("#" + id).append('<div role="treeitem" id="item-' + item_id + '" class="list-group-node" data-path="' + path + json[i].text + '" data-bs-toggle="collapse" data-bs-target="#' + item_id + '" style="padding-left: ' + ((size * nivel) + 5) + 'px"></div>');
                $("#" + id).append('<span role="group" class="list-group collapse" id="' + item_id + '"></span>');
                
                $("#" + id + " #item-" + item_id).append('<div class="float-start flex"></div>');
                $("#" + id + " #item-" + item_id + " .float-start").append('<i class="state-icon bi bi-folder"></i>');
                $("#" + id + " #item-" + item_id + " .float-start").append(json[i].text);
                
                $("#" + id + " #item-" + item_id).append('<div class="float-end flex"></div>');
                $("#" + id + " #item-" + item_id + " .float-end").append('<i id="new-' + item_id + '" class="bi bi-file-plus-fill item-new"></i>');
                $("#" + id + " #item-" + item_id + " .float-end").append('<i id="delete-' + item_id + '" class="bi bi-trash item-delete"></i>');
                
                generateTreeView(item_id, json[i].nodes, nivel + 1, path + json[i].text + "/");
            } else {
                $("#" + id).append('<div role="treeitem" id="item-' + item_id + '" class="list-group-node" data-path="' + path + json[i].text + '" data-bs-toggle="collapse" data-bs-target="#' + item_id + '" style="padding-left: ' + ((size * nivel) + 5) + 'px"></div>');
                
                $("#" + id + " #item-" + item_id).append('<div class="float-start flex"></div>');
                $("#" + id + " #item-" + item_id + " .float-start").append('<i class="bi bi-folder-x"></i>');
                $("#" + id + " #item-" + item_id + " .float-start").append(json[i].text);
                
                $("#" + id + " #item-" + item_id).append('<div class="float-end flex"></div>');
                $("#" + id + " #item-" + item_id + " .float-end").append('<i id="new-' + item_id + '" class="bi bi-file-plus-fill item-new"></i>');
                $("#" + id + " #item-" + item_id + " .float-end").append('<i id="delete-' + item_id + '" class="bi bi-trash item-delete"></i>');
            }
        } else {
            $("#" + id).prepend('<div role="treeitem" id="item-' + item_id + '" class="list-group-node" data-path="' + path + json[i].text + '" data-bs-toggle="collapse" data-bs-target="#' + item_id + '" style="padding-left: ' + ((size * nivel) + 5) + 'px"></div>');
            
            $("#" + id + " #item-" + item_id).append('<div class="float-start flex"></div>');
            $("#" + id + " #item-" + item_id + " .float-start").append('<i class="bi bi-filetype-json"></i>');
            $("#" + id + " #item-" + item_id + " .float-start").append(json[i].text);

            $("#" + id + " #item-" + item_id).append('<div class="float-end flex"></div>');
            $("#" + id + " #item-" + item_id + " .float-end").append('<i id="open-' + item_id + '" class="bi bi-box-arrow-down item-open"></i>');
            $("#" + id + " #item-" + item_id + " .float-end").append('<i id="delete-' + item_id + '" class="bi bi-trash item-delete"></i>');
        }
    }

    $(".list-group-node").unbind();
    $(".list-group-node").click(function () {
        if ($(this).find(".state-icon").hasClass("bi-folder")) {
            $(this).find(".state-icon").removeClass("bi-folder").addClass("bi-folder2-open");
        } else {
            $(this).find(".state-icon").removeClass("bi-folder2-open").addClass("bi-folder");
        }
    });

    var list_group_id = "";
    
    $(".item-new").unbind();
    $(".item-new").click(function (e) {
        var path = $(this).parent().parent().attr("data-path");
        $("#path-nfc").html(path + "/");
        e.stopPropagation();
    });

    $(".item-new").mouseenter(function () {
        list_group_id = $(this).parent().parent().attr("data-bs-target");
        $(this).parent().parent().removeAttr("data-bs-target");
    });

    $(".item-new").mouseleave(function () {
        $(this).parent().parent().attr({"data-bs-target": list_group_id});
    });
    
    $(".item-open").unbind();
    $(".item-open").click(function () {
        var path = $(this).parent().parent().attr("data-path");
        console.log(path);
    });
    
    $(".item-delete").unbind();
    $(".item-delete").click(function () {
        var id = $(this).attr("id").split("-");
        var path = $(this).parent().parent().attr("data-path");
        request("/DELETE_FILE", {path: path}, function (response) {
            if (response == "ok") {
                $("#item-" + id[1]).remove();
                notify("success", "Arquivo removido com sucesso.");
            } else {
                notify("error", "O arquivo não pode ser excluído.");
            }
        }, function (response) {
            notify("error", "Houve um problema com a comunicação.");
        });
    });
}

function popover_elements() {

    $(".popover").unbind();
    $(".popover").each(function() {
        $(this).popover('hide');
    });
    
    $(".pop").popover('dispose');
    $('.pop').popover({
        container: 'body',
        placement: 'bottom',
        trigger: 'hover',
        boundary: 'window'
    });

    $(".pop").mouseleave(function() {
        $(this).popover('hide');
    });

    $(".pop").click(function() {
        $(this).popover('hide');
    });

}

var GetUrl = window.location;
var ws = new WebSocket("ws://" + GetUrl.host + "/socket");

$(document).ready(function () {

    $(window).bind("load resize scroll", function () {
        isMobile();
    });

    $("div").each(function () {

        let include = $(this).attr("include-html");

        if (include != undefined) {

            let setid = String(include).replace(/\/|\.html/g, (string) => {
                if (string == ".html") {
                    return "";
                } else {
                    return "-";
                }
            });

            $(this).removeAttr("include-html").attr({ "id": setid });

            request(include, {}, function (r) {
                $("#" + setid).after(r);
                $("#" + setid).remove();
                EventsReload();
                popover_elements();
            }, function () {
                console.log("Erro ao carregar: " + include);
                // notificar(mensages["error"]["load-module"], mensages["error"]["load-module-title"], 3000, "error");
            });

        }

    });

})