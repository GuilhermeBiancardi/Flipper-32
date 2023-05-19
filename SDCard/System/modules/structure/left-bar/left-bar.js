/* global page */

$(document).ready(function () {
    $("#side-menu li").click(function () {
        var click_page = $(this).attr("id");
        if (page != click_page) {
            carregaPagina(click_page, this);
        } else {
            toogleBarraEsquerda('close');
        }
    });
});