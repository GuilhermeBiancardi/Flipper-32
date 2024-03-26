function request(url, method, data, success = "", error = "") {

    var request = $.ajax({
        url: url,
        method: method,
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
        request("modules/pages/" + page_load + "/" + page_load + ".html", "GET", {}, function(response) {
            $("#modules").html(response);
            window.setTimeout(function() {
                EventsReload();
                if(!isMobile()) {
                    popoverElements();
                }
            }, 250);
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
    return /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
}

function mobileUpdate() {
    if ($(window).width() < 800 || isMobile()) {
        $('body').addClass('app-mobile');
        $('body').addClass('menu-toggler-on');
    } else {
        $('body').removeClass('app-mobile');
        $('body').removeClass('menu-toggler-on');
    }
}

function notify(type, message) {
    if (type == "success") {
        $("#toast-success-message").html(message);
        $("#toast-success").toast("show");
    } else if (type == "error") {
        $("#toast-error-message").html(message);
        $("#toast-error").toast("show");
    } else if (type == "info") {
        $("#toast-info-message").html(message);
        $("#toast-info").toast("show");
    } else if (type == "warning") {
        $("#toast-warning-message").html(message);
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

function isHex(text) {
    const hexRegex = /^[0-9A-Fa-f]+$/g;
    return hexRegex.test(text);
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

function getDateTime() {
    var dataAtual = new Date();
    var dia = String(dataAtual.getDate()).padStart(2, '0');
    var mes = String(dataAtual.getMonth() + 1).padStart(2, '0'); // Os meses são indexados a partir de 0
    var ano = dataAtual.getFullYear();
    var horas = String(dataAtual.getHours()).padStart(2, '0');
    var minutos = String(dataAtual.getMinutes()).padStart(2, '0');
    var segundos = String(dataAtual.getSeconds()).padStart(2, '0');
    return dia + "/" + mes + "/" + ano + " " + horas + ":" + minutos + ":" + segundos;
}

function menuOpen(obj, id, aIn = "fadeInLeft", aOut = "fadeOutLeft") {
    animateMenu(obj, id, aIn, aOut);
}

function animateMenu(obj, targetID, aIn = "fadeInLeft", aOut = "fadeOutLeft") {
    if ($(obj).hasClass("btn-secondary")) {
        $(obj).removeClass("btn-secondary").addClass("btn-outline-secondary");
        $("#" + targetID).removeClass(aIn).addClass(aOut);
        window.setTimeout(() => {
            $("#" + targetID).toggleClass("d-none");
        }, 500);
    } else {
        $(obj).removeClass("btn-outline-secondary").addClass("btn-secondary");
        $("#" + targetID).removeClass(aOut).addClass(aIn);
        $("#" + targetID).toggleClass("d-none");
    }
}

function popoverElements() {

    $(".popover").unbind();
    $(".popover").each(function() {
        $(this).popover('hide');
    });
    
    $(".pop").popover('dispose');
    $('.pop').popover({
        container: 'body',
        placement: 'top',
        trigger: 'hover',
        boundary: 'window'
    });

    $(".pop").mouseleave(function() {
        $(".popover").remove();
    });
}

function listDirSDCard(id_tree_view, data, path, success = "", error = "") {
    request("/LIST_DIR", "POST", {path: path}, function (response) {
        if (isJson(response)) {
            var json = JSON.parse(response);
            $("#" + id_tree_view).html("");
            generateTreeView(id_tree_view, json.root, 0, data, path + "/", (response) => {
                if (success != "") {
                    success(response);
                }
            });
        } else {
            notify("error", "O modo leitura não pode ser ligado.");
            if (error != "") {
                error();
            }
        }
    }, function (response) {
        notify("error", "Houve um problema com a comunicação.");
    });
}

function sdcardPrompt(pathLabel, placeholder, buttonName, callback) {
    swal("O diretório de criação será: \n'" + pathLabel + "':", {
        content: {
            element: "input",
            attributes: {
                type: "text",
                class: "form-control",
                placeholder: placeholder
            }
        },
        buttons: {
            cancel: {
                text: "Cancelar",
                className: "btn btn-danger",
                visible: true,
            }, 
            confirm: {
                text: buttonName,
                className: "btn btn-theme",
                visible: true,
                closeModal: true,
            },
        },
    }).then((value) => {
        if(value) {
            callback(value);
        }
    });
}

function generateTreeView(id, json, nivel = 0, idDataElement, path = "", callbackFileOpen = "") {

    var size = 25.75;
    var idAux = id;

    if(nivel == 0) {
        var id_root = remove_accents(path.replaceAll(/\ |\\|\//g, "").toLowerCase());
        $("#" + id).append('<div role="treeitem" id="item-list_' + id_root + '" class="list-group-node" data-path="' + id_root.toUpperCase() + '" data-bs-toggle="collapse" data-bs-target="#list_' + id_root + '" style="padding-left: ' + ((size * nivel) + 5) + 'px"></div>');
        $("#" + id).append('<span role="group" class="list-group collapse" id="list_' + id_root + '"></span>');            
        
        $("#" + id + " #item-list_" + id_root).append('<div class="float-start flex"></div>');
        $("#" + id + " #item-list_" + id_root + " .float-start").append('<i class="state-icon bi bi-folder"></i>');
        $("#" + id + " #item-list_" + id_root + " .float-start").append(id_root.toUpperCase());
        
        $("#" + id + " #item-list_" + id_root).append('<div class="float-end flex"></div>');
        $("#" + id + " #item-list_" + id_root + " .float-end").append('<i id="add-' + id_root + '" class="bi bi-folder-plus item-add-folder pop" data-bs-content="Nova Pasta"></i>');
        $("#" + id + " #item-list_" + id_root + " .float-end").append('<i id="remove-' + id_root + '" class="bi bi-file-plus item-add-file pop" data-bs-content="Novo Arquivo"></i>');
        
        id = "list_" + id_root;
        nivel++;
    }

    if(json != "empy") {
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
                    $("#" + id + " #item-" + item_id + " .float-end").append('<i id="add-' + item_id + '" class="bi bi-folder-plus item-add-folder pop" data-bs-content="Nova Pasta"></i>');
                    $("#" + id + " #item-" + item_id + " .float-end").append('<i id="remove-' + item_id + '" class="bi bi-file-plus item-add-file pop" data-bs-content="Novo Arquivo"></i>');
                    $("#" + id + " #item-" + item_id + " .float-end").append('<i id="delete-' + item_id + '" class="bi bi-trash2 item-delete pop" data-bs-content="Excluir Pasta"></i>');
                    
                    generateTreeView(item_id, json[i].nodes, nivel + 1, idDataElement, path + json[i].text + "/");
                } else {
                    $("#" + id).append('<div role="treeitem" id="item-' + item_id + '" class="list-group-node" data-path="' + path + json[i].text + '" data-bs-toggle="collapse" data-bs-target="#' + item_id + '" style="padding-left: ' + ((size * nivel) + 5) + 'px"></div>');
                    
                    $("#" + id + " #item-" + item_id).append('<div class="float-start flex"></div>');
                    $("#" + id + " #item-" + item_id + " .float-start").append('<i class="bi bi-folder-x"></i>');
                    $("#" + id + " #item-" + item_id + " .float-start").append(json[i].text);
                    
                    $("#" + id + " #item-" + item_id).append('<div class="float-end flex"></div>');
                    $("#" + id + " #item-" + item_id + " .float-end").append('<i id="add-' + item_id + '" class="bi bi-folder-plus item-add-folder pop" data-bs-content="Nova Pasta"></i>');
                    $("#" + id + " #item-" + item_id + " .float-end").append('<i id="remove-' + item_id + '" class="bi bi-file-plus item-add-file pop" data-bs-content="Novo Arquivo"></i>');
                    $("#" + id + " #item-" + item_id + " .float-end").append('<i id="delete-' + item_id + '" class="bi bi-trash2 item-delete pop" data-bs-content="Excluir Pasta"></i>');
                }
            } else {
                $("#" + id).prepend('<div role="treeitem" id="item-' + item_id + '" class="list-group-node" data-path="' + path + json[i].text + '" data-bs-toggle="collapse" data-bs-target="#' + item_id + '" style="padding-left: ' + ((size * nivel) + 5) + 'px"></div>');
                
                $("#" + id + " #item-" + item_id).append('<div class="float-start flex"></div>');
                $("#" + id + " #item-" + item_id + " .float-start").append('<i class="bi bi-filetype-json"></i>');
                $("#" + id + " #item-" + item_id + " .float-start").append(json[i].text);

                $("#" + id + " #item-" + item_id).append('<div class="float-end flex"></div>');
                $("#" + id + " #item-" + item_id + " .float-end").append('<i id="save-' + item_id + '" class="bi bi-floppy item-save pop" data-bs-content="Salvar Dados"></i>');
                $("#" + id + " #item-" + item_id + " .float-end").append('<i id="open-' + item_id + '" class="bi bi-download item-open pop" data-bs-content="Carregar Dados"></i>');
                $("#" + id + " #item-" + item_id + " .float-end").append('<i id="delete-' + item_id + '" class="bi bi-trash2 item-delete pop" data-bs-content="Excluir Arquivo"></i>');
            }
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
    
    $(".item-add-folder").unbind();
    $(".item-add-folder").click(function (e) {
        e.stopPropagation();
        var pathLabel = $(this).parent().parent().attr("data-path");
        sdcardPrompt(pathLabel, "Nome da Pasta", "Criar Pasta", (value) => {
            var path_request = pathLabel + "/" + value;
            request("/CREATE_FOLDER", "POST", { path: path_request }, function (response) {
                if (response == "ok") {
                    notify("success", "Pasta criada com sucesso.");
                    listDirSDCard(idAux, idDataElement, path.replaceAll(/\//g, ""), function () {
                        var path_dir = pathLabel.split("/");
                        var dir = "";
                        $("#list_" + remove_accents(path.replaceAll(/\ |\\|\//g, "").toLowerCase())).collapse("toggle");
                        for (var j = 1; j < path_dir.length; j++) {
                            dir += "_" + remove_accents(path_dir[j].replaceAll(/\ |\\/g, "_").toLowerCase());
                            $("#" + remove_accents(path.replaceAll(/\ |\\|\//g, "").toLowerCase()) + dir).collapse("toggle");
                        }
                    });
                } else {
                    notify("error", "A pasta não pode ser criada.");
                }
            }, function () {
                notify("error", "Houve um problema com a comunicação.");
            });
        });
    });
    
    $(".item-add-file").unbind();
    $(".item-add-file").click(function (e) {
        e.stopPropagation();
        var pathLabel = $(this).parent().parent().attr("data-path");
        sdcardPrompt(pathLabel, "Nome do Arquivo", "Criar Arquivo", (value) => {
            var path_request = pathLabel + "/" + value;
            request("/CREATE_FILE", "POST", { path: path_request }, function (response) {
                if (response == "ok") {
                    notify("success", "Arquivo criado com sucesso.");
                    listDirSDCard(idAux, idDataElement, path.replaceAll(/\//g, ""), function () {
                        var path_dir = pathLabel.split("/");
                        var dir = "";
                        $("#list_" + remove_accents(path.replaceAll(/\ |\\|\//g, "").toLowerCase())).collapse("toggle");
                        for (var j = 1; j < path_dir.length; j++) {
                            dir += "_" + remove_accents(path_dir[j].replaceAll(/\ |\\/g, "_").toLowerCase());
                            $("#" + remove_accents(path.replaceAll(/\ |\\|\//g, "").toLowerCase()) + dir).collapse("toggle");
                        }
                    });
                } else {
                    notify("error", "O arquivo não pode ser criado.");
                }
            }, function () {
                notify("error", "Houve um problema com a comunicação.");
            });
        });
    });

    $(".item-add-file, .item-add-folder").mouseenter(function () {
        list_group_id = $(this).parent().parent().attr("data-bs-target");
        $(this).parent().parent().removeAttr("data-bs-target");
    });

    $(".item-add-file, .item-add-folder").mouseleave(function () {
        $(this).parent().parent().attr({"data-bs-target": list_group_id});
    });
    
    $(".item-open").unbind();
    $(".item-open").click(function () {
        var path = $(this).parent().parent().attr("data-path");
        request("/FILE_OPEN", "POST", {path: path}, function (response) {
            if(callbackFileOpen != "") {
                callbackFileOpen(response);
            }
        }, function () {
            notify("error", "Houve um problema com a comunicação.");
        });
    });
    
    $(".item-save").unbind();
    $(".item-save").click(function () {
        var path = $(this).parent().parent().attr("data-path");
        request("/FILE_SAVE", "POST", {path: path, data: $("#" + idDataElement).val()}, function (response) {
            if (isJson(response)) {
                var json = JSON.parse(response);
                notify(json.status, json.message);
            }
        }, function () {
            notify("error", "Houve um problema com a comunicação.");
        });
    });
    
    $(".item-delete").unbind();
    $(".item-delete").click(function () {
        var id = $(this).attr("id").split("-");
        var path = $(this).parent().parent().attr("data-path");
        request("/DELETE_FILE", "POST", {path: path}, function (response) {
            if (response == "ok") {
                $("#item-" + id[1]).remove();
                notify("success", "Arquivo removido com sucesso.");
            } else {
                notify("error", "O arquivo não pode ser excluído.");
            }
        }, function () {
            notify("error", "Houve um problema com a comunicação.");
        });
    });

    if(!isMobile()) {
        popoverElements();
    }

}

var GetUrl = window.location;
var ws = new WebSocket("ws://" + GetUrl.host + "/socket");

$(document).ready(function () {

    mobileUpdate();

    $(window).bind("load resize", function () {
        mobileUpdate();
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

            request(include, "GET", {}, function (r) {
                $("#" + setid).after(r);
                $("#" + setid).remove();
                EventsReload();
                if(!isMobile()) {
                    popoverElements();
                }
            }, function () {
                console.log("Erro ao carregar: " + include);
                // notificar(messages["error"]["load-module"], messages["error"]["load-module-title"], 3000, "error");
            });

        }

    });

})