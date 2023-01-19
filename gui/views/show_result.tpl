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
            <tr> <td> Eff. focal length: </td><td>{{str(round(efective_focal_length,1))}} mm </td> </tr>
        </tbody>
    </table>
</div>
%end


<center style="padding-bottom: 3rem">
    <a href="/?index_file={{index_file}}">

    <input type="button" value="Go back" />
    </a>
</center>

% if success and annotated_photo:

<center style="padding-bottom: 1rem">
<h5>Annotated photo</h5>
</center>
<div class="container" style="width:50%;padding-bottom:3rem">
        <a href="{{annotated_photo}}">
        <img src="{{annotated_photo}}"
        width="100%"
        title="Annotated Photo"
        >
        </a>

</div>
%else:
<center style="padding-bottom: 4rem">

</center>
%end

% include('footer.tpl')