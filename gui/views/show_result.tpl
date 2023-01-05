% include('header.tpl')

<center style="padding-bottom: 2rem">
Plate-solving {{"finished" if success else "failed"}} in {{str(round(time_to_platesolve,3))}} s <br>
</center>

% if success:

<div class="container" style="width:50%">
    <table class='table table-sm result-table'>
        <tbody>
            <tr> <td> Right ascension </td><td> {{RA}}</td> </tr>
            <tr> <td> Declination </td><td> {{dec}}</td> </tr>
            <tr> <td> Camera rotation </td><td> {{rot}}</td> </tr>
            <tr> <td> Photo angular size </td><td>{{width}} x {{height}} </td> </tr>
        </tbody>
    </table>
</div>
%end

<center style="padding-bottom: 7rem">
    <a href="/?index_file={{index_file}}">

    <input type="button" value="Go back" />
    </a>
</center>

% include('footer.tpl')