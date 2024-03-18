$(document).ready(function () {
    $('#menu-esquerdo').click(function () {
        toogleBarraEsquerda('');
        toogleBarraDireita('close');
    });
    $('#menu-direito').click(function () {
        toogleBarraDireita('');
        toogleBarraEsquerda('close');
    });
});